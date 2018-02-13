/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <graphene/app/application.hpp>
#include <graphene/app/plugin.hpp>

#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/protocol/transaction.hpp>

#include <graphene/utilities/tempdir.hpp>

#include <graphene/account_history/account_history_plugin.hpp>
#include <graphene/witness/witness.hpp>
#include <graphene/market_history/market_history_plugin.hpp>
#include <graphene/egenesis/egenesis.hpp>
#include <graphene/wallet/wallet.hpp>

#include <fc/thread/thread.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/rpc/websocket_api.hpp>
#include <fc/rpc/cli.hpp>

#include <boost/filesystem/path.hpp>

#define BOOST_TEST_MODULE Test Application
#include <boost/test/included/unit_test.hpp>

using namespace graphene;

// hack:  import create_example_genesis() even though it's a way, way
// specific internal detail
namespace graphene { namespace app { namespace detail {
	graphene::chain::genesis_state_type create_example_genesis();
} } } // graphene::app::detail

boost::filesystem::path create_genesis_file(fc::temp_directory& directory) {
	boost::filesystem::path genesis_path = boost::filesystem::path{directory.path().generic_string()} / "genesis.json";
    fc::path genesis_out = genesis_path;
    graphene::chain::genesis_state_type genesis_state = graphene::app::detail::create_example_genesis();
    std::cerr << "Creating example genesis state in file " << genesis_out.generic_string() << "\n";
    fc::json::save_to_file(genesis_state, genesis_out);
    return genesis_path;
}

/**
 * Start a server and connect using the same calls as the CLI
 */
BOOST_AUTO_TEST_CASE( cli_connect )
{
   using namespace graphene::chain;
   using namespace graphene::app;
   try {
      BOOST_TEST_MESSAGE( "Creating temporary files" );

      fc::temp_directory app_dir( graphene::utilities::temp_directory_path() );

      BOOST_TEST_MESSAGE( "Creating and initializing app1" );

      graphene::app::application app1;
      app1.register_plugin<graphene::account_history::account_history_plugin>();
      app1.register_plugin< graphene::market_history::market_history_plugin >();
      app1.register_plugin< graphene::witness_plugin::witness_plugin >();
      app1.startup_plugins();
      boost::program_options::variables_map cfg;
      cfg.emplace("rpc-endpoint", boost::program_options::variable_value(string("127.0.0.1:8090"), false));
      cfg.emplace("genesis-json", boost::program_options::variable_value(create_genesis_file(app_dir), false));
      app1.initialize(app_dir.path(), cfg);

      BOOST_TEST_MESSAGE( "Starting app1 and waiting 500 ms" );
      app1.startup();
      fc::usleep(fc::milliseconds(500));

      chain_id_type chain_id = app1.chain_database()->get_chain_id();

      // connect to the server
      graphene::wallet::wallet_data wdata;
      wdata.chain_id = chain_id;
      wdata.ws_server = "ws://127.0.0.1:8090";
      wdata.ws_user = "";
      wdata.ws_password = "";
      fc::http::websocket_client client;
      auto con  = client.connect( wdata.ws_server );

      auto apic = std::make_shared<fc::rpc::websocket_api_connection>(*con);

      auto remote_api = apic->get_remote_api< login_api >(1);
      BOOST_CHECK(remote_api->login( wdata.ws_user, wdata.ws_password ) );

      auto wapiptr = std::make_shared<graphene::wallet::wallet_api>( wdata, remote_api );
      //TODO: throw the wallet.json in the temp directory. Clean up after yourself!
      wapiptr->set_wallet_filename( "/home/parallels/Development2/cpp/bitshares-core/programs/cli_wallet/wallet.json" );
      wapiptr->load_wallet_file();

      fc::api<graphene::wallet::wallet_api> wapi(wapiptr);

      auto wallet_cli = std::make_shared<fc::rpc::cli>();
      for( auto& name_formatter : wapiptr->get_result_formatters() )
         wallet_cli->format_result( name_formatter.first, name_formatter.second );

      boost::signals2::scoped_connection closed_connection(con->closed.connect([=]{
         cerr << "Server has disconnected us.\n";
         wallet_cli->stop();
      }));
      (void)(closed_connection);

      // set password for opening the wallet
      wapiptr->set_password("supersecret");
      wapiptr->unlock("supersecret");
      BOOST_CHECK_EQUAL("nathan", wapiptr->get_account("nathan").name);
      // import Nathan's private key
      BOOST_CHECK(wapiptr->import_key("nathan", "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"));
      // import Nathan's balances
      std::vector<std::string> keyVector;
      keyVector.push_back("5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3");
      std::vector<signed_transaction> transactions = wapiptr->import_balance("nathan", keyVector, true);
      // Nathan must be Lifetime Member to create other accounts
      signed_transaction upgrade_tx = wapiptr->upgrade_account("nathan", true);
      // create a new account
      graphene::wallet::brain_key_info bki = wapiptr->suggest_brain_key();
      BOOST_CHECK(!bki.brain_priv_key.empty());
      wapiptr->create_account_with_brain_key(bki.brain_priv_key, "jmjatlanta", "nathan", "nathan", true);
      // Have Nathan give some BTS to jmjatlanta
      wapiptr->transfer("nathan", "jmjatlanta", "10000", "BTS", "Here is some BTS", true);
      // create a new coin
      asset_options common;
      fc::optional<bitasset_options> options;
      common.description = "Issue 582 Coin";
      common.core_exchange_rate = graphene::chain::price::unit_price();
      common.core_exchange_rate.base.asset_id = wapiptr->get_asset("BTS").id;
      common.core_exchange_rate.base.amount = 21;
      common.core_exchange_rate.quote.asset_id = asset_id_type(1);
      common.core_exchange_rate.quote.amount = 76399;
      common.flags = charge_market_fee
		 | override_authority
		 | transfer_restricted
		 | disable_force_settle
		 | disable_confidential
		 | witness_fed_asset;
      signed_transaction asset_tx = wapiptr->create_asset("nathan", "ISSUE582COIN", 2, common, options, true);
      BOOST_CHECK(asset_tx.ref_block_num > 0);
      //TODO: issue some of the new coin to jmjatlanta
      signed_transaction new_asset_tx = wapiptr->issue_asset("nathan", "10000", "ISSUE582COIN", "New Coin Issued", true);
      BOOST_CHECK(new_asset_tx.ref_block_num > 0);
      //TODO: attempt to trade new asset for BTS
      signed_transaction sell_coin_tx = wapiptr->sell_asset("nathan", "1", "ISSUE582COIN", "100", "BTS", 10000, false, true);
      BOOST_CHECK(sell_coin_tx.ref_block_num > 0);
      //TODO: Attempt to trade BTS
      signed_transaction sell_bts_tx = wapiptr->sell_asset("nathan", "1", "BTS", "1", "ISSUE582COIN", 10000, false, true);
      BOOST_CHECK(sell_bts_tx.ref_block_num > 0);
   } catch( fc::exception& e ) {
      edump((e.to_detail_string()));
      throw;
   }
}

/*
 * Copyright (c) 2019 Michel Santos and contributors.
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

#include <boost/test/unit_test.hpp>
#include "../common/database_fixture.hpp"
#include <graphene/chain/loan_offer_object.hpp>
#include <graphene/protocol/margin_trading_market.hpp>

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE(margin_trading_tests, database_fixture)

   /****
    * Reference Price Basic Tests
    */

   /****
    * Offer Tests
    */
   BOOST_AUTO_TEST_CASE(submit_offer_operation_1) {
      try {
         ACTORS((alice)(feedproducer)(nathan));

         int64_t init_balance(100 * GRAPHENE_BLOCKCHAIN_PRECISION);
         int64_t large_init_balance(400000 * GRAPHENE_BLOCKCHAIN_PRECISION);

         // Define core asset
         const auto &core = asset_id_type()(db);
         asset_id_type core_id = core.id;

         // Create a smart asset
         const auto &bitusd = create_bitasset("USDBIT", feedproducer_id);
         asset_id_type usd_id = bitusd.id;
         update_feed_producers(bitusd, {feedproducer.id});
         price_feed current_feed;
         current_feed.maintenance_collateral_ratio = 1750;
         current_feed.maximum_short_squeeze_ratio = 1100;
         current_feed.settlement_price = bitusd.amount(1) / core.amount(5);
         publish_feed(bitusd, feedproducer, current_feed);

         // Create a UIA
         upgrade_to_lifetime_member( nathan );
         const asset_id_type uia_id = create_user_issued_asset( "NATHANCOIN", nathan, white_list ).id;
         issue_uia( alice_id, asset( init_balance, uia_id ) );

         // Fund alice with core asset
         transfer(committee_account, alice_id, graphene::chain::asset(init_balance));
         // Alice will borrow 1000 bitUSD in exchange for 150start out with 300% collateral, call price is 15/1.75 CORE/USD = 60/7
         const call_order_object& call = *borrow( alice_id, bitusd.amount(1000), asset(15000));

         // clear everything out
         // generate_blocks(HARDFORK_XXX_TIME); // Advance to after margin trading is activated
         generate_block();
         trx.clear();

         // Alice puts offer on the blockchain
         // Alice offer to borrow USD to trade against a UIA
         {
            loan_offer_create_operation offer_op = loan_offer_create_operation();
            offer_op.lender = alice_id;
            offer_op.loan_asset = usd_id;
            offer_op.tradable_asset = uia_id;
            offer_op.min_to_lend = 400;
            offer_op.max_to_lend = 800;
            offer_op.min_interest_rate = 2500;  // 25% APR

            offer_op.min_duration = 7; // days
            offer_op.max_duration = 30; // days
            offer_op.max_call_duration = 86400; // seconds

            // MCR and MCCR are expressed in the same way as in the existing price feed logic
            // i. e. only the part above 100%, and as a scaled percentage value. E. g. a value of `4200` would mean `142%`.
            offer_op.mccr = 3000; // 30% + 100%  = 130% = 1.30x
            offer_op.mcr = 11000; // 110% + 100% = 210% = 2.10x
            offer_op.expiration = db.head_block_time() + fc::minutes(60);

//            offer_op.fee = db.get_global_properties().parameters.current_fees->calculate_fee(offer_op);
            std::cout << "Basic offer test" << std::endl;

            trx.operations.push_back(offer_op);
            set_expiration( db, trx );
            sign(trx, alice_private_key);
            PUSH_TX(db, trx, ~0);

            generate_block();
         }

      }
      FC_LOG_AND_RETHROW()
   }

   /****
    * Offer tests after reference price becomes invalid
    */

   /****
    * Offer tests when asset-pairs are "blacklisted" by asset issuer
    * Test blacklisting
    * Test de-whitelisting
    */

   /****
    * Offer tests when accounts are "blacklisted" by asset issuer
    * Test blacklisting
    * Test de-whitelisting
    */

   /****
    * Tests for edge case: as MCCR <= 0
    */

   /****
    * Tests for edge case: reference price <= 0
    */

   /****
    * Tests for edge case: daily interest = 0
    */

   /****
    * Margin Trading Tests
    */

   /****
    * Margin trading tests after asset-pairs are "blacklisted" by asset issuer
    */

   /****
    * Margin trading tests after accounts are "blacklisted" by asset issuer
    */

   /****
    * Margin trading tests unaffected after asset seizure by asset issuer
    */

   /****
    * Margin trading tests after reference price becomes invalid
    */

   /****
    * Loan Portfolio Update by Deposit Tests
    */

   /****
    * Loan Portfolio Update by Deposit Tests after account is blacklisted by asset issuer
    */

   /****
    * Loan Portfolio Update by Withdrawal Tests after account is blacklisted by asset issuer
    */

   /****
    * Loan Portfolio Interest Tests
    */

   /****
    * Loan Portfolio Update after reference price becomes invalid
    */

   /****
    * Margin call triggered by inability to pay interest tests
    */

   /****
    * Margin call triggered by inability to pay interest tests after reference price becomes invalid
    */

   /**
    * Margin call triggered by reduction of reference price tests
    */

   /**
    * Margin call not triggered after reference becomes invalid
    */

   /****
    * Conventional loan closure by expiration tests
    */

   /****
    * Conventional loan closure by early payment by borrower tests
    */

   /****
    * Conventional loan closure after reference price becomes invalid
    */

   /****
    * Unconventional loan closure triggered by expiration of margin call tests
    */

   /****
    * Unconventional loan closure triggered by expiration of margin call tests after reference price becomes invalid
    */

   /****
    * Unconventional loan closure triggered by expiration of margin call tests after account is "blacklisted" by asset issuer
    */

BOOST_AUTO_TEST_SUITE_END()
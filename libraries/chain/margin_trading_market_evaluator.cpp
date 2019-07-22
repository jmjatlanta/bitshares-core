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
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/market_object.hpp>

#include <graphene/chain/margin_trading_market_evaluator.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

#include <graphene/protocol/market.hpp>

#include <fc/uint128.hpp>
#include <graphene/chain/loan_offer_object.hpp>
#include <iostream>


namespace graphene {
   namespace chain {
      void_result loan_offer_create_evaluator::do_evaluate(const loan_offer_create_operation &op) {
         try {
            const database &d = db();

            std::cout << "Head block time:" << std::endl;
            std::cout << d.head_block_time().to_iso_string() << std::endl;
            std::cout << "Op expiration:" << std::endl;
            std::cout << op.expiration.to_iso_string() << std::endl;

            FC_ASSERT(op.expiration >= d.head_block_time());

            // TODO Check balance of lender
            // TODO Check Whitelisting/not-blacklisting of loan asset
            // TODO Check Whitelisting/not-blacklisting of borrow asset
            // TODO Check trading pair restrictions


            return void_result();
         } FC_CAPTURE_AND_RETHROW((op))
      }

      object_id_type loan_offer_create_evaluator::do_apply(const loan_offer_create_operation &op) {
         try {//
            const auto &new_order_object = db().create<loan_offer_object>([&](loan_offer_object &obj) {
               // TODO: Set object values equal to operation values
            });

            loan_offer_id_type order_id = new_order_object.id; // save this because we may remove the object by filling it
            // TODO: Apply the offer to the database
            // TODO: Take different actions depending the filling of the offer

            return order_id;

         } FC_CAPTURE_AND_RETHROW((op))
      }

   }
} // graphene::chain

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
#pragma once
#include <graphene/protocol/base.hpp>
#include <graphene/protocol/asset.hpp>

namespace graphene { namespace protocol { 

   /**
    *  @class loan_offer_create_operation
    *  @brief Instructs the blockchain to create a loan loan offer that could be matched with a borrow offer to create a loan for margin trading
    *  @ingroup operations
    *
    */
   struct loan_offer_create_operation : public base_operation
   {
      struct fee_parameters_type { uint64_t fee = 5 * GRAPHENE_BLOCKCHAIN_PRECISION; };

      asset           fee;

      account_id_type lender;
      asset_id_type loan_asset;
      share_type min_to_lend;
      share_type max_to_lend;

      // MCR and MCCR are expressed in the same way as in the existing price feed logic
      // i. e. only the part above 100%, and as a scaled percentage value. E. g. a value of `4200` would mean `142%`.
      unsigned_int mcr;
      unsigned_int mccr;

      unsigned_int max_call_duration; // seconds
      asset_id_type tradable_asset;
      unsigned_int min_duration; // days
      unsigned_int max_duration; // days
      unsigned_int min_interest_rate;

      time_point_sec   expiration;
      // extensions_type        ext;

      account_id_type fee_payer()const { return lender; }
      void            validate()const;

   };

} } // graphene::protocol

FC_REFLECT( graphene::protocol::loan_offer_create_operation::fee_parameters_type, (fee) )

FC_REFLECT( graphene::protocol::loan_offer_create_operation,(fee)(lender)(loan_asset)(min_to_lend)(max_to_lend)(mcr)(mccr)(max_call_duration)(tradable_asset)(min_duration)(max_duration)(min_interest_rate)(expiration))

GRAPHENE_DECLARE_EXTERNAL_SERIALIZATION( graphene::protocol::loan_offer_create_operation::fee_parameters_type )
GRAPHENE_DECLARE_EXTERNAL_SERIALIZATION( graphene::protocol::loan_offer_create_operation )

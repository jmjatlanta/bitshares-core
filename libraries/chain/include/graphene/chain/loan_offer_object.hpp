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

#include <graphene/chain/types.hpp>
#include <graphene/db/generic_index.hpp>
#include <graphene/protocol/asset.hpp>
#include <graphene/protocol/base.hpp>

#include <boost/multi_index/composite_key.hpp>

namespace graphene {
   namespace chain {

/**
 *  @brief An offer to lend an amount of an asset into a margin trading portfolio
 *  @ingroup object
 *  @ingroup protocol
 *  @ingroup market
 */
      class loan_offer_object : public abstract_object<loan_offer_object> {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id = loan_offer_object_type;

         account_id_type owner;
         asset_id_type loan_asset;
         share_type min_amount;
         share_type max_amount;
         unsigned_int mcr;
         unsigned_int mccr;
         unsigned_int max_call_duration;
         asset_id_type tradable_asset;
         unsigned_int min_duration;
         unsigned_int max_duration;
         unsigned_int min_interest_rate;

         time_point_sec expiration;
         // extensions_type        ext;

      };

      struct by_price;
      struct by_expiration;
      struct by_account;
      typedef multi_index_container <
      loan_offer_object,
      indexed_by<
              ordered_unique < tag < by_id>, member<object, object_id_type, &object::id>>
//      ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
//      ordered_unique< tag<by_expiration>,
//         composite_key< loan_offer_object,
//            member< loan_offer_object, time_point_sec, &loan_offer_object::expiration>,
//            member< object, object_id_type, &object::id>
//         >
//      >
      >
      >
      loan_offer_multi_index_type;

      typedef generic_index <loan_offer_object, loan_offer_multi_index_type> loan_offer_index;

   }
} // graphene::chain

MAP_OBJECT_ID_TO_TYPE(graphene::chain::loan_offer_object)

FC_REFLECT_TYPENAME(graphene::chain::loan_offer_object)

GRAPHENE_DECLARE_EXTERNAL_SERIALIZATION(graphene::chain::loan_offer_object)

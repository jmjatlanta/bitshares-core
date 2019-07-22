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
#include <graphene/protocol/margin_trading_market.hpp>

#include <fc/io/raw.hpp>
#include <graphene/protocol/chain_parameters.hpp>

namespace graphene { namespace protocol {

void loan_offer_create_operation::validate()const
{
   FC_ASSERT( fee.amount >= 0 );

   FC_ASSERT( min_to_lend > 0 );
   FC_ASSERT( max_to_lend > min_to_lend );

   FC_ASSERT( min_duration > 0 );
   FC_ASSERT( max_duration >= min_duration );
   // TODO: Test very small loan durations.  Prevent very small loan durations if they are problematic.

//   graphene::chain::database& d = db();
//   chain_parameters chain_parameters = d.get_global_properties().parameters;

//   FC_ASSERT( mccr > chain_parameters.min_mccr );
//   FC_ASSERT( mcr > chain_parameters.min_mcr );
   FC_ASSERT( mcr >= mccr );

   FC_ASSERT( min_interest_rate >= 0 );
//   FC_ASSERT( min_interest_rate <= chain_parameters.max_interest_rate );
//   FC_ASSERT( max_call_duration <= chain_parameters.max_call_duration );

   // Check properties of the assets
   FC_ASSERT( loan_asset != tradable_asset );
   // TODO: loan_asset must not be transfer-restricted
   // TODO: tradable_asset must not be transfer-restricted

   // Check properties of the lenders
   // TODO: Lender must be a valid account ID, must exit, must authorize the operation, must pay fee
   // TODO: Lender must have at least max_to_lend in his balance

   // Check the lender's access to assets
   // TODO: Lender must be whitelisted or not blacklisted for the loan_asset
   // TODO: Lender must be whitelisted or not blacklisted for the tradable_asset

}

} } // graphene::protocol

GRAPHENE_IMPLEMENT_EXTERNAL_SERIALIZATION( graphene::protocol::loan_offer_create_operation::fee_parameters_type )
GRAPHENE_IMPLEMENT_EXTERNAL_SERIALIZATION( graphene::protocol::loan_offer_create_operation )

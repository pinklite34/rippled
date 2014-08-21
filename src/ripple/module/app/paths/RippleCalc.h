//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_RIPPLECALC_H
#define RIPPLE_RIPPLECALC_H

namespace ripple {
namespace path {

/** RippleCalc calculates the quality of a payment path.

    Quality is the amount of input required to produce a given output along a
    specified path - another name for this is exchange rate.
*/
class RippleCalc
{
public:
    struct Input
    {
        bool partialPaymentAllowed = false;
        bool defaultPathsAllowed = true;
        bool limitQuality = false;
        bool deleteUnfundedOffers = false;
        bool isLedgerOpen = true;
    };
    struct Output
    {
        // The computed input amount.
        STAmount actualAmountIn;

        // The computed output amount.
        STAmount actualAmountOut;

        // Expanded path with all the actual nodes in it.
        // A path starts with the source account, ends with the destination account
        // and goes through other acounts or order books.
        PathState::List pathStateList;

    private:
        TER calculationResult_;

    public:
        TER result () const
        {
            return calculationResult_;
        }
        void setResult (TER const value)
        {
            calculationResult_ = value;
        }

    };

    static Output rippleCalculate (
        LedgerEntrySet& activeLedger,

        // Compute paths using this ledger entry set.  Up to caller to actually
        // apply to ledger.

        // Issuer:
        //      XRP: xrpAccount()
        //  non-XRP: uSrcAccountID (for any issuer) or another account with
        //           trust node.
        STAmount const& saMaxAmountReq,             // --> -1 = no limit.

        // Issuer:
        //      XRP: xrpAccount()
        //  non-XRP: uDstAccountID (for any issuer) or another account with
        //           trust node.
        STAmount const& saDstAmountReq,

        Account const& uDstAccountID,
        Account const& uSrcAccountID,

        // A set of paths that are included in the transaction that we'll
        // explore for liquidity.
        STPathSet const& spsPaths,
        Input const* const pInputs = nullptr);

    /** The active ledger. */
    LedgerEntrySet& mActiveLedger;

    // If the transaction fails to meet some constraint, still need to delete
    // unfunded offers.
    //
    // Offers that were found unfunded.
    path::OfferSet permanentlyUnfundedOffers_;

    // First time working in reverse a funding source was mentioned.  Source may
    // only be used there.

    // Map of currency, issuer to node index.
    AccountIssueToNodeIndex mumSource_;

private:
    RippleCalc (
        LedgerEntrySet& activeLedger,
        STAmount const& saMaxAmountReq,             // --> -1 = no limit.
        STAmount const& saDstAmountReq,

        Account const& uDstAccountID,
        Account const& uSrcAccountID,
        STPathSet const& spsPaths)
            : mActiveLedger (activeLedger),
              saDstAmountReq_(saDstAmountReq),
              saMaxAmountReq_(saMaxAmountReq),
              uDstAccountID_(uDstAccountID),
              uSrcAccountID_(uSrcAccountID),
              spsPaths_(spsPaths)
    {
    }

    /** Compute liquidity through these path sets. */
    TER rippleCalculate ();

    /** Add a single PathState.  Returns true on success.*/
    bool addPathState(STPath const&, TER&);

    STAmount const& saDstAmountReq_;
    STAmount const& saMaxAmountReq_;
    Account const& uDstAccountID_;
    Account const& uSrcAccountID_;
    STPathSet const& spsPaths_;

    // The computed input amount.
    STAmount actualAmountIn_;

    // The computed output amount.
    STAmount actualAmountOut_;

    // Expanded path with all the actual nodes in it.
    // A path starts with the source account, ends with the destination account
    // and goes through other acounts or order books.
    PathState::List pathStateList_;

    Input inputFlags;
};

} // path
} // ripple

#endif

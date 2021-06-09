/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_nonce_tracker.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "base/bind.h"
#include "brave/components/brave_wallet/browser/eth_address.h"
#include "brave/components/brave_wallet/browser/eth_json_rpc_controller.h"
#include "brave/components/brave_wallet/browser/eth_tx_state_manager.h"

namespace brave_wallet {

namespace {

uint256_t GetHighestLocallyConfirmed(
    const std::vector<EthTxStateManager::TxMeta> metas) {
  uint256_t highest = 0;
  for (auto& meta : metas) {
    highest = std::max(highest, meta.tx.nonce());
  }
  return ++highest;
}

uint256_t GetHighestContinuousFrom(
    const std::vector<EthTxStateManager::TxMeta> metas,
    uint256_t start) {
  uint256_t highest = start;
  for (auto& meta : metas) {
    if (meta.tx.nonce() == highest)
      highest++;
  }
  return highest;
}

}  // namespace

EthNonceTracker::EthNonceTracker(EthTxStateManager* tx_state_manager,
                                 EthJsonRpcController* rpc_controller)
    : tx_state_manager_(tx_state_manager),
      rpc_controller_(rpc_controller),
      weak_factory_(this) {}
EthNonceTracker::~EthNonceTracker() = default;

bool EthNonceTracker::GetNextNonce(const EthAddress& from, uint256_t* nonce) {
  DCHECK(nonce);
  const std::string hex_address = from.ToHex();
  if (network_nonce_map_.find(hex_address) == network_nonce_map_.end()) {
    rpc_controller_->GetTransactionCount(
        hex_address,
        base::BindOnce(&EthNonceTracker::OnGetNetworkNonce,
                       weak_factory_.GetWeakPtr(), std::move(hex_address)));
    return false;
  }
  if (!nonce_lock_.Try())
    return false;
  uint256_t network_nonce = network_nonce_map_[hex_address];
  network_nonce_map_.erase(hex_address);
  auto confirmed_transactions = tx_state_manager_->GetTransactionsByStatus(
      EthTxStateManager::TransactionStatus::CONFIRMED, from);
  uint256_t local_highest = GetHighestLocallyConfirmed(confirmed_transactions);

  uint256_t highest_confirmed = std::max(network_nonce, local_highest);

  auto pending_transactions = tx_state_manager_->GetTransactionsByStatus(
      EthTxStateManager::TransactionStatus::SUBMITTED, from);

  uint256_t highest_continuous_from =
      GetHighestContinuousFrom(pending_transactions, highest_confirmed);

  *nonce = std::max(network_nonce, highest_continuous_from);

  nonce_lock_.Release();
  return true;
}

void EthNonceTracker::OnGetNetworkNonce(std::string address,
                                        bool status,
                                        uint256_t result) {
  if (!status)
    return;
  network_nonce_map_[address] = result;
}

}  // namespace brave_wallet

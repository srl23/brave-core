/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_URL_LOADER_COMPONENT_URL_LOADER_COMPONENT_H_
#define BRAVE_COMPONENTS_URL_LOADER_COMPONENT_URL_LOADER_COMPONENT_H_

#include <list>
#include <map>
#include <memory>
#include <string>

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "url/gurl.h"

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
}  // namespace network

namespace url_loader_component {

class URLLoaderComponent {
 public:
  URLLoaderComponent(
      net::NetworkTrafficAnnotationTag annotation_tag,
      scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory);
  ~URLLoaderComponent();

  using URLRequestCallback =
      base::OnceCallback<void(const int,
                              const std::string&,
                              const std::map<std::string, std::string>&)>;
  void Request(const std::string& method, const GURL& url,
               const std::string& payload,
               const std::string& payload_content_type,
               bool auto_retry_on_network_change,
               URLRequestCallback callback);

 private:
  using SimpleURLLoaderList =
      std::list<std::unique_ptr<network::SimpleURLLoader>>;
  void OnURLLoaderComplete(SimpleURLLoaderList::iterator iter,
                           URLRequestCallback callback,
                           const std::unique_ptr<std::string> response_body);

  net::NetworkTrafficAnnotationTag annotation_tag_;
  SimpleURLLoaderList url_loaders_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;
  base::WeakPtrFactory<URLLoaderComponent> weak_ptr_factory_;
};

}  // namespace url_loader_component

#endif  // BRAVE_COMPONENTS_URL_LOADER_COMPONENT_URL_LOADER_COMPONENT_H_

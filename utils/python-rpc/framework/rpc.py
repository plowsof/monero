# Copyright (c) The Monero Project
# 
# This source code is licensed under the BSD-3 license found in the
# LICENSE file in the root directory of this source tree.

# 

import requests
import json

class Response(dict):
    def __init__(self, d):
        for k in d.keys():
            if type(d[k]) == dict:
                self[k] = Response(d[k])
            elif type(d[k]) == list:
                self[k] = []
                for i in range(len(d[k])):
                    if type(d[k][i]) == dict:
                        self[k].append(Response(d[k][i]))
                    else:
                        self[k].append(d[k][i])
            else:
                self[k] = d[k]

    def __getattr__(self, key):
        return self[key]
    def __setattr__(self, key, value):
        self[key] = value
    def __eq__(self, other):
        if type(other) == dict:
            return self == Response(other)
        if self.keys() != other.keys():
            return False
        for k in self.keys():
            if self[k] != other[k]:
                return False
        return True

class JSONRPC(object):
    def __init__(self, url):
        self.url = url

    def send_request(self, path, inputs, result_field = None):
        res = requests.post(
            self.url + path,
            data=json.dumps(inputs),
            headers={'content-type': 'application/json'})
        res = res.json()
        
        assert 'error' not in res, res

        if result_field:
            res = res[result_field]

        return Response(res)

    def send_json_rpc_request(self, inputs):
        return self.send_request("/json_rpc", inputs, 'result')




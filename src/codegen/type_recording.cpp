// Copyright (c) 2014 Dropbox, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <unordered_map>

#include "core/options.h"
#include "core/types.h"

#include "codegen/type_recording.h"

namespace pyston {

static std::unordered_map<AST*, TypeRecorder*> type_recorders;
TypeRecorder* getTypeRecorderForNode(AST* node) {
    TypeRecorder*& r = type_recorders[node];
    if (r == NULL)
        r = new TypeRecorder();
    return r;
}

Box* recordType(TypeRecorder* self, Box* obj) {
    BoxedClass* cls = obj->cls;
    if (cls != self->last_seen) {
        self->last_seen = cls;
        self->last_count = 1;
    } else {
        self->last_count++;
    }

    // printf("Seen %s %ld times\n", getNameOfClass(cls)->c_str(), self->last_count);

    return obj;
}

BoxedClass* predictClassFor(AST* node) {
    auto it = type_recorders.find(node);
    if (it == type_recorders.end())
        return NULL;

    TypeRecorder* r = it->second;
    return r->predict();
}

BoxedClass* TypeRecorder::predict() {
    if (!ENABLE_TYPE_FEEDBACK)
        return NULL;

    if (last_count > 100)
        return last_seen;

    return NULL;
}
}

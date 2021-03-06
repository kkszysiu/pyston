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

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "core/common.h"
#include "core/types.h"

#include "codegen/codegen.h"

#include "gc/collector.h"
#include "gc/heap.h"
#include "gc/root_finder.h"

#ifndef NVALGRIND
#include "valgrind.h"
#endif

namespace pyston {
namespace gc {

// unsigned numAllocs = 0;
unsigned bytesAllocatedSinceCollection = 0;

static TraceStack roots;
void registerStaticRootObj(void* obj) {
    assert(global_heap.getAllocationFromInteriorPointer(obj));
    roots.push(obj);
}

bool TraceStackGCVisitor::isValid(void* p) {
    return global_heap.getAllocationFromInteriorPointer(p);
}

inline void TraceStackGCVisitor::_visit(void* p) {
    assert(isValid(p));
    stack->push(p);
}

void TraceStackGCVisitor::visit(void* p) {
    _visit(p);
}

void TraceStackGCVisitor::visitRange(void* const* start, void* const* end) {
#ifndef NDEBUG
    void* const* cur = start;
    while (cur < end) {
        assert(isValid(*cur));
        cur++;
    }
#endif
    stack->pushall(start, end);
}

void TraceStackGCVisitor::visitPotential(void* p) {
    void* a = global_heap.getAllocationFromInteriorPointer(p);
    if (a) {
        visit(a);
    }
}

void TraceStackGCVisitor::visitPotentialRange(void* const* start, void* const* end) {
    while (start < end) {
        visitPotential(*start);
        start++;
    }
}

#define MAX_KINDS 1024
#define KIND_OFFSET 0x111
static kindid_t num_kinds = 0;
static AllocationKind::GCHandler handlers[MAX_KINDS];

extern "C" kindid_t registerKind(const AllocationKind* kind) {
    assert(kind == &untracked_kind || kind->gc_handler);
    assert(num_kinds < MAX_KINDS);
    assert(handlers[num_kinds] == NULL);
    handlers[num_kinds] = kind->gc_handler;
    return KIND_OFFSET + num_kinds++;
}

static void markPhase() {
#ifndef NVALGRIND
    // Have valgrind close its eyes while we do the conservative stack and data scanning,
    // since we'll be looking at potentially-uninitialized values:
    VALGRIND_DISABLE_ERROR_REPORTING;
#endif

    TraceStack stack(roots);
    collectStackRoots(&stack);

    TraceStackGCVisitor visitor(&stack);

    // if (VERBOSITY()) printf("Found %d roots\n", stack.size());
    while (void* p = stack.pop()) {
        assert(((intptr_t)p) % 8 == 0);
        GCObjectHeader* header = headerFromObject(p);
        // printf("%p\n", p);

        if (isMarked(header)) {
            // printf("Already marked, skipping\n");
            continue;
        }

        // printf("Marking + scanning %p\n", p);

        setMark(header);

        ASSERT(KIND_OFFSET <= header->kind_id && header->kind_id < KIND_OFFSET + num_kinds, "%p %d", header,
               header->kind_id);

        if (header->kind_id == untracked_kind.kind_id)
            continue;

        // ASSERT(kind->_cookie == AllocationKind::COOKIE, "%lx %lx", kind->_cookie, AllocationKind::COOKIE);
        // AllocationKind::GCHandler gcf = kind->gc_handler;
        AllocationKind::GCHandler gcf = handlers[header->kind_id - KIND_OFFSET];

        assert(gcf);
        // if (!gcf) {
        // std::string name = g.func_addr_registry.getFuncNameAtAddress((void*)kind, true);
        // ASSERT(gcf, "%p %s", kind, name.c_str());
        //}

        gcf(&visitor, p);
    }

#ifndef NVALGRIND
    VALGRIND_ENABLE_ERROR_REPORTING;
#endif
}

static void sweepPhase() {
    global_heap.freeUnmarked();
}

static int ncollections = 0;
void runCollection() {
    static StatCounter sc("gc_collections");
    sc.log();

    if (VERBOSITY("gc") >= 2)
        printf("Collection #%d\n", ++ncollections);

    // if (ncollections == 754) {
    // raise(SIGTRAP);
    //}

    markPhase();
    sweepPhase();
}

} // namespace gc
} // namespace pyston

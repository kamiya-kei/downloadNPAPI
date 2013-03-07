// Copyright 2008 Google Inc.
#pragma once

#include "common.h"

namespace glue {
namespace globals {

class NPAPIObject : public NPObject {
 public:
  explicit NPAPIObject(NPP npp);
  ~NPAPIObject();
  void set_base(NPAPIObject *base) { base_ = base; }
  NPAPIObject *base() { return base_; }
  void set_names(NPIdentifier *names) { names_ = names; }
  NPIdentifier *names() { return names_; }
  int count() { return count_; }
  NPP npp() {return npp_;}
  void AllocateNamespaceObjects(int count);
  void SetNamespaceObject(int i, NPAPIObject *object) {
    namespaces_[i] = object;
  }
  NPAPIObject *GetNamespaceObjectByIndex(int i) {
    return namespaces_[i];
  }
  NPAPIObject *GetNamespaceObject(NPIdentifier name) {
    DebugScopedId id(name);  // debug helper
    for (int i = 0; i < count_; ++i)
      if (name == names_[i])
        return namespaces_[i];
    return NULL;
  }
 private:
  NPP npp_;
  NPAPIObject **namespaces_;
  NPIdentifier *names_;
  int count_;
  NPAPIObject *base_;
};

NPObject *Allocate(NPP npp, NPClass *theClass);
void Deallocate(NPObject *header);
bool HasProperty(NPObject *header, NPIdentifier name);
bool GetProperty(NPObject *header, NPIdentifier name,
                 NPVariant *variant);
bool SetProperty(NPObject *header, NPIdentifier name,
                 const NPVariant *variant);

}  // namespace globals
}  // namespace glue


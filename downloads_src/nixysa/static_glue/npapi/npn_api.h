#ifndef TOOLS_IDLGLUE_NG_STATIC_GLUE_NPAPI_NPN_API_H_
#define TOOLS_IDLGLUE_NG_STATIC_GLUE_NPAPI_NPN_API_H_

#include <npfunctions.h>

// Initializes the NPN API, by copying the browser functions given by the
// browser.
// NOTE: this function will fail if the browser doesn't support NPRuntime
// (minor version at least 14).
// Parameters:
//   functions: the function table provided by the browser on plug-in
//   initialization.
// Returns:
//   NPERR_NO_ERROR if no error.
//   NPERR_INVALID_FUNCTABLE_ERROR if the function table passed in has an
//   incorrect size
//   NPERR_INCOMPATIBLE_VERSION_ERROR if the major NPAPI version in the
//   function table doesn't match the expected one
NPError InitializeNPNApi(NPNetscapeFuncs *functions);

// Checks whether NPN_HasProperty is implemented with a slower work-around.
// Safari claims to support NPRuntime, but doesn't implement NPN_HasProperty,
// so we use a work-around using NPN_Enumerate, but it is (much) slower.
// This function allows the user to test for this case (and avoid
// NPN_HasProperty if possible).
bool IsHasPropertyWorkaround();

// Checks whether NPN_PluginThreadAsyncCall is implemented correctly.
// Safari 4 on Mac OS X claims it is supported, but provides a NULL
// function pointer for it. Even worse, Safari 5 on Mac OS X does not even
// initialize the function pointer.
bool IsPluginThreadAsyncCallSupported(NPP instance);

#endif  // TOOLS_IDLGLUE_NG_STATIC_GLUE_NPAPI_NPN_API_H_

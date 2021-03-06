// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_SHELL_CHILD_PROCESS_H_
#define MOJO_SHELL_CHILD_PROCESS_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "mojo/system/embedder/scoped_platform_handle.h"

class CommandLine;

namespace mojo {
namespace shell {

// A base class for child processes -- i.e., code that is actually run within
// the child process. (Instances are manufactured by |Create()|.)
class ChildProcess {
 public:
  enum Type {
    // TODO(vtl): Add real types here.
    TYPE_TEST
  };

  // Returns null if the command line doesn't indicate that this is a child
  // process. |main()| should call this, and if it returns non-null it should
  // call |Run()| inside a main message loop.
  static scoped_ptr<ChildProcess> Create(const CommandLine& command_line);

  void Run();

  virtual ~ChildProcess();

  // To be implemented by subclasses. This is the "entrypoint" for a child
  // process.
  virtual void Main() = 0;

 protected:
  ChildProcess();

  embedder::ScopedPlatformHandle* platform_channel() {
    return &platform_channel_;
  }

 private:
  // Available in |Main()| (after |Run()|).
  embedder::ScopedPlatformHandle platform_channel_;

  DISALLOW_COPY_AND_ASSIGN(ChildProcess);
};

}  // namespace shell
}  // namespace mojo

#endif  // MOJO_SHELL_CHILD_PROCESS_H_

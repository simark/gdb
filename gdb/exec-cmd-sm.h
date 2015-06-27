/* Execution command state machine, for GDB, the GNU debugger.
   Copyright (C) 2015 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef EXEC_CMD_SM_H
#define EXEC_CMD_SM_H

struct return_value_info;

/* To continue the execution commands when running gdb asynchronously.
   A continuation structure contains a pointer to a function to be
   called to finish the command, once the target has stopped.  */

struct exec_cmd_sm;

struct exec_cmd_sm_ops
{
  /* The destructor.  */
  void (*dtor) (struct exec_cmd_sm *self);

  /* Called after handle_inferior_event decides the target is
     done.  */
  int (*should_stop) (struct exec_cmd_sm *self);

  void (*clean_up) (struct exec_cmd_sm *self);

  struct return_value_info *(*return_value) (struct exec_cmd_sm *self);
};

struct exec_cmd_sm
{
  struct exec_cmd_sm_ops *ops;
};

extern void exec_cmd_sm_ctor (struct exec_cmd_sm *self, struct exec_cmd_sm_ops *ops);

#endif /* EXEC_CMD_SM_H */

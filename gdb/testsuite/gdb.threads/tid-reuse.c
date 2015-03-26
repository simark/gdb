/* This testcase is part of GDB, the GNU debugger.

   Copyright 2015 Free Software Foundation, Inc.

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

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

unsigned long thread_counter;

void *
do_nothing_thread_func (void *arg)
{
  thread_counter++;
  usleep (1);
  return NULL;
}

void *
spawner_thread_func (void *arg)
{
  while (1)
    {
      pthread_t child;
      int rc;

      rc = pthread_create (&child, NULL, do_nothing_thread_func, NULL);
      assert (rc == 0);

      rc = pthread_join (child, NULL);
      assert (rc == 0);
    }

  return NULL;
}

void
after_count (void)
{
}

void
after_wrap (void)
{
}


unsigned int wrap_time = -1;
int tid_wrap = -1;

#ifdef __linux__

static int
linux_proc_get_pid_max (void)
{
  static const char filename[]  ="/proc/sys/kernel/pid_max";
  FILE *file;
  char buf[100];
  int retval = -1;

  file = fopen (filename, "r");
  if (file == NULL)
    {
      fprintf (stderr, "unable to open %s\n", filename);
      return -1;
    }

  if (fgets (buf, sizeof (buf), file) != NULL)
    retval = strtol (buf, NULL, 10);

  fclose (file);
  return retval;
}

#endif

int
main (int argc, char *argv[])
{
  pthread_t child;
  int rc;

  rc = pthread_create (&child, NULL, spawner_thread_func, NULL);
  assert (rc == 0);

#define COUNT_SLEEP 2
  sleep (COUNT_SLEEP);

#ifdef __linux__
  tid_wrap = linux_proc_get_pid_max ();
#endif
  /* If we don't know how many threads it would take to wrap around on
     this system, just run the test for a bit.  */
  if (tid_wrap > 0)
    wrap_time = (float) tid_wrap / (thread_counter / COUNT_SLEEP) + 3;

  /* 4 seconds were sufficient on the machine this was first observed,
     an Intel i7-2620M @ 2.70GHz running Linux 3.18.7, with
     pid_max=32768.  Supposedly going forward, faster machines will
     need less time, unless of course, if pid_max is set to a very
     high number.  To avoid unreasonably long test time, cap to an
     upper bound.  */
  if (wrap_time > 60)
    wrap_time = 60;
  printf ("thread_counter=%lu, tid_wrap = %u, wrap_time=%u\n",
	  thread_counter, tid_wrap, wrap_time);
  after_count ();

  sleep (wrap_time);

  after_wrap ();
  return 0;
}

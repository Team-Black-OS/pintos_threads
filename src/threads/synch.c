/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
     static int entered = 0;
void
sema_init (struct semaphore *sema, unsigned value)
{
  ASSERT (sema != NULL);

  sema->value = value;
  list_init (&sema->waiters);
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  while (sema->value == 0)
    {
      // Changed the list_push_back call into a call to list_insert_ordered().
      // Same process we used for alarm clock.
      list_insert_ordered (&sema->waiters, &thread_current ()->elem,&thr_less,NULL);
      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema)
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0)
    {
      sema->value--;
      success = true;
    }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  struct thread *unblk_thrd = NULL;
  if (!list_empty (&sema->waiters)){
    unblk_thrd = list_entry(list_pop_back (&sema->waiters),struct thread,elem);
    thread_unblock (unblk_thrd);
  }
  sema->value++;
      intr_set_level (old_level);
  //printf("Before check.\n");
  if(!intr_context() && unblk_thrd && unblk_thrd->priority > thread_current()->priority){
    thread_yield();
  }
  //printf("After check.\n");
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void)
{
  struct semaphore sema[2];
  int i;

  printf ("Testing semaphores...");
  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++)
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
  printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_)
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++)
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);

  lock->holder = NULL;
  sema_init (&lock->semaphore, 1);
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  // PRIORITY DONATION TK
  enum intr_level old_level;
  // disable interrupts

  old_level = intr_disable ();
  // Pointer to current thread.
  struct thread* current_thread = thread_current();
  // Pointer to current lock holder.
  struct thread* lock_holder = lock->holder;
  // Pointer to current lock.
  struct lock* current_lock = lock;
  //printf("Entered for %d time\n",entered);
  //=========================================TRY TO DONATE PRIORITY
  if(current_thread != NULL && lock_holder != NULL && lock_holder->priority < current_thread->priority){

      current_thread->blocked_on = lock;
      //printf("Thread %s is waiting on Thread %s",current_thread->name,current_thread->blocked_on->holder->name);
      // Insert current thread into the list containing thread donors for the thread holding the lock.
      list_insert_ordered(&lock_holder->thread_donors,&current_thread->prior_elem,&thr_prior_less,NULL);
      
      

       struct thread* crd = list_entry(list_back(&lock_holder->thread_donors),struct thread,prior_elem);
      lock_holder->priority = crd->priority;


      while(lock_holder->blocked_on){
        //printf("%s is waiting on %s\n",current_thread->name,lock_holder->name);
        struct thread* hold = lock_holder->blocked_on->holder;

        list_sort(&hold->thread_donors,&thr_prior_less,NULL);

        hold->priority = list_entry(list_back(&hold->thread_donors),struct thread,prior_elem)->priority;

        lock_holder = hold;
      }
  }
  //=============================================================*/
  // turn interrurpts back on


 // END PRIORITY DONATION

  sema_down (&lock->semaphore);

  lock->holder = thread_current();
 // lock->holder->blocked_on = NULL;
  intr_set_level (old_level);

}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  success = sema_try_down (&lock->semaphore);
  if (success)
    lock->holder = thread_current ();
  return success;
}

/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  enum intr_level old_level;
  old_level = intr_disable();
  struct thread* current = thread_current();

  //======================SEES IF PRIORITY WAS DONATED, IF SO GOES BACK TO ORIGINAL PRIORITY
  // If we have one or more donated priorities
  if(!list_empty(&current->thread_donors)){
   // printf("Current Thread: %d\n TID: %d\n Thread Magic %x\n",current->priority, current->tid,current->magic);
   // printf("List size %d\n",list_size(&current->thread_donors));
    struct list_elem *e;
    // Loop through each thread that donated a priority
    for (e = list_begin (&current->thread_donors); e != list_end (&current->thread_donors); e = list_next (e))
    {

      struct thread *t = list_entry (e, struct thread, prior_elem);
      // If this thread donated its priority because of the lock we're releasing, remove this thread
      // from the list of donors.
      if(t->blocked_on == lock){
      list_remove(&t->prior_elem);
      }
    }
    // If the list is now empty,
    if(list_empty(&current->thread_donors)){
      // Return our current priority to the base priority.
      current->priority = current->base_priority;
    }else{
      // Get the next highest priority, and set our new priority.
      current->priority = list_entry(list_back(&current->thread_donors),struct thread,prior_elem)->priority;
    }
   
  }

  //===================================================================================*/

  //lock->holder->blocked_on = NULL;
  lock->holder = NULL;
  sema_up (&lock->semaphore);
  intr_set_level(old_level);
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock)
{
  ASSERT (lock != NULL);

  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem
  {
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);

  list_init (&cond->waiters);
}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock)
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  sema_init (&waiter.semaphore, 0);
  list_push_back (&cond->waiters, &waiter.elem);
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}
// Function returns true if the first semaphore's highest priority thread is lower priority than
// the second semaphore's highest priority thread.
bool cond_less(const struct list_elem *first, const struct list_elem *second, void *aux){
  struct semaphore* one = &list_entry(first,struct semaphore_elem,elem)->semaphore;
  struct semaphore* two = &list_entry(second,struct semaphore_elem,elem)->semaphore;
  int priority_one = list_entry(list_back(&(one->waiters)),struct thread,elem)->priority;
  int priority_two = list_entry(list_back(&(two->waiters)),struct thread,elem)->priority;
 // printf("Priority: %d, Priority %d\n",priority_one,priority_two);
  return(priority_one < priority_two);
}
/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters))
    sema_up (&list_entry (list_pop_max (&cond->waiters,&cond_less,NULL),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}

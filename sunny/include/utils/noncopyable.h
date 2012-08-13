/* Copyright 2002-2005 Evertz Microsystems Ltd. All rights reserved.
 * $Id: noncopyable.h,v 1.1.8.1 2010-02-25 21:13:18 bliu Exp $
 *-------------------------------------------------------------------------*/
/**
 * @file noncopyable.h
 *
 *
 * @brief define a basic class to be inherited for those don't want to be copied 
 * A base class that guarantees that derived classes cannot be copied
 * This is done by defining a private copy constructor and a private 
 * copy assignment operator.
 *
 * @author Bin Liu
 *
 * @todo
 *
 *
 * @see other references...
 */
/*--------------------------------------------------------------------------*/
#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class Noncopyable
{
protected:
   /**
    * A constructor which does nothing
    */
   Noncopyable(){}   

   /**
    * A destructor which does nothing 
    */
   ~Noncopyable(){}	

private:
   /**
    *  private copy constructor to make this class non-copyable 
    */
   Noncopyable(const Noncopyable &){}

   /**
    * private copy assignment operator
    */
   const Noncopyable& operator = (const Noncopyable&) { return *this; }

};

#endif


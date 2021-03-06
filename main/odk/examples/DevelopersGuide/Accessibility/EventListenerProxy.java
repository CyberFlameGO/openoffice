/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleEventListener;
import com.sun.star.accessibility.AccessibleEventObject;
import com.sun.star.lang.EventObject;
import com.sun.star.awt.XTopWindowListener;
import com.sun.star.uno.UnoRuntime;

import java.util.LinkedList;

/** This class acts as a proxy for the simple screen reader.  It waits for
    two types of events: 
    1. Accessibility events signal modifications concerning accessibility
       objects.
    2. Top window events inform the listener about new or removed windows.

    This class exists because events had to be handled in a seperate thread
    to avoid dead locks: The thread that receives an event must no call back
    to the Office directly.  

    Soon this should not be necessary anymore.  There is now a flag which 
    switches between synchronous and asynchronous callbacks.

    All reveived events are eventually forwarded to the actual listener.  In
    this way it decouples the Office from the listener.
*/
class EventListenerProxy
    implements Runnable, XAccessibleEventListener, XTopWindowListener
{
    public EventListenerProxy (EventHandler aListener)
    {
        maListener = aListener;
        mbAsynchron = true;
        if (mbAsynchron)
        {
            maEventQueue = new LinkedList();
            new Thread (this, "EventListenerProxy").start();
        }
    }

    private void addEvent (Runnable aEventObject)
    {
        if (mbAsynchron)
            synchronized (maEventQueue)
            {
                maEventQueue.addLast (aEventObject);
                // Tell the queue that there is a new event in the queue.
                maEventQueue.notify();
            }
        else
        {
            System.out.println ("running " + aEventObject);
            aEventObject.run();
            System.out.println ("  done");
        }
    }




    /** In an infinite loop, check for events to deliver, then wait on lock
        (which will be notified when new events arrive)
    */
    public void run ()
    {
        while (true)
        {
            // Process all events that are currently in the queue.
            Runnable aEvent;
            do
            {
                synchronized (maEventQueue)
                {
                    if (maEventQueue.size() > 0)
                        aEvent = (Runnable)maEventQueue.removeFirst();
                    else
                        aEvent = null;
                }
 
                if (aEvent != null)
                {
                    try
                    {
                        aEvent.run();
                    }
                    catch (Throwable aException)
                    {
                        MessageArea.println( 
                            "Exception during event delivery: " + aException);
                        aException.printStackTrace();
                    }
                }
            }
            while (aEvent != null);

            // Now that the queue is empty go to sleep again.
            try
            {
                synchronized (maEventQueue)
                {
                    maEventQueue.wait();
                }
            }
            catch (Exception aException)
            {
                // Ignore this exception since there is not much 
                // that we can do about it.
            }
        }
    }


    public void disposing( final EventObject aEvent)
    {
        addEvent (new Runnable() 
            {
                public void run() 
                {
                    maListener.disposing (aEvent);
                }
            } );
    }

    public void notifyEvent (final AccessibleEventObject aEvent)
    {
        addEvent (
            new Runnable()
            {
                public void run()
                {
                    maListener.notifyEvent (aEvent);
                }
            } );
    }

    public void windowOpened (final com.sun.star.lang.EventObject aEvent)
    {
        addEvent (
            new Runnable()
            {
                public void run()
                {
                    maListener.windowOpened (
                        (XAccessible) UnoRuntime.queryInterface(
                            XAccessible.class,
                            aEvent.Source));
                }
            } );
    }
    public void windowClosing (final com.sun.star.lang.EventObject aEvent)
    {
        // Ignored.
    }
    public void windowClosed (final com.sun.star.lang.EventObject aEvent)
    {
        addEvent (
            new Runnable()
            {
                public void run()
                {
                    maListener.windowClosed (
                        (XAccessible) UnoRuntime.queryInterface(
                            XAccessible.class,
                            aEvent.Source));
                }
                } );
    }
    public void windowMinimized (com.sun.star.lang.EventObject aEvent)
    {
        // Ignored.
    }
    public void windowNormalized (com.sun.star.lang.EventObject aEvent)
    {
        // Ignored.
    }
    public void windowActivated (com.sun.star.lang.EventObject aEvent)
    {
        // Ignored.
    }
    public void windowDeactivated (com.sun.star.lang.EventObject aEvent)
    {
        // Ignored.
    }

    /** The queue of event objects, LinkedList<Runnable>.
        The queue object will also serve as lock for the consumer/producer type
        syncronization.
    */
    private LinkedList maEventQueue;

    /** This is the actual listener that the events will eventually forwarded to.
    */
    private EventHandler maListener;

    /** This flag determines whether event forwarding is done asynchroniously
        or synchroniously.
    */
    private boolean mbAsynchron;
}

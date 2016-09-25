/* 1. PQUEUE will crash if there are less than three paragraph
 * 2. QThread destroyed while still running error. try if we can manually terminate QThread. (FIXED)
 * 3. "DONE" dialog show up immediately after processing a second file. (FIXED)
 * 4. SaveReport doesnt work after processing second time. (FIXED)
 * 5. move the saving to file process to worker object and move into a new thread, otherwise the waitnotify box gui will keep looping and won't save. (FIXED)
 * 6. reporttabwidget does not have setmodal (so that you can only interact with tabwidget while it is up, not the main gui. the report widget also freezes the main gui,
 *      nothing i can do, you cannot move Qwidget to another thread other than main thread. tab widget is also memory intensive...
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */


//Improvement and sugggestions
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */

// Lesson Learned
/* 1. when finishing worker process in separate thread, move it back to main thread. then move to savethread.
 * 2. "Although QObject is reentrant, the GUI classes, notably QWidget and all its subclasses, are not reentrant. They can only be used from the main thread."
 * 3. QObject must not have parent otherwise it cannot move to another thrad
 *
 *
 *
 *
 *
 *
 */

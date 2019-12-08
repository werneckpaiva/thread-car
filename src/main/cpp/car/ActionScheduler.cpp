/*
 * ActionScheduler.cpp - A library for creating timed events/actions.
 * Created by Aleksandr N. Mirchev, Aug 27, 2017.
 * Released into the public domain.
 */

#include "ActionScheduler.h"

int ActionScheduler::maxActions;
ActionScheduler ** ActionScheduler::actions;

void ActionScheduler::start(unsigned long startInterval, bool startNow) {
    static bool init = false;
    if (!init) {
        maxActions = 15;
        actions = new ActionScheduler*[maxActions]; //start at 15 actions, increase as needed
        for (int i = 0; i < maxActions; i++) {
            actions[i] = NULL;
        }
        init = true;
    }

    if (index == -1) {
        int emptyIndex = -1;
        for (int i = 0; i < maxActions; i++) {
            if (actions[i] == NULL) {
                emptyIndex = i;
                break;
            }
        }

        if (emptyIndex == -1) { //unable to find spot
            emptyIndex = maxActions;
            int previousMax = maxActions;
            maxActions += 5;
            ActionScheduler ** newArray = new ActionScheduler*[maxActions];
            for (int i = previousMax; i < maxActions; i++) {
                newArray[i] = NULL;
            }
            for (int i = 0; i < previousMax; i++) {
                newArray[i] = actions[i];
            }
            delete [] actions;
            actions = newArray;
        }

        actions[emptyIndex] = this;
        index = emptyIndex;
        interval = startInterval;
        if (startNow) {
            nextTick = millis();
        } else {
            nextTick = millis() + interval;
        }
        running = true;
    } else {
        interval = startInterval;
        if (!running) {
            toggleRunning();
        }
    }
}

void ActionScheduler::toggleRunning() {
    if (index != -1) {
        running = !running;
        if (running) {
            nextTick = millis();
        }
    }
}

void ActionScheduler::continueRunning(unsigned long nextInterval) {
  if (index != -1) {
      running = true;
      nextTick = millis();
      if (nextInterval != 0) {
          interval = nextInterval;
          nextTick += nextInterval;
      }
  }
}

void ActionScheduler::pause() {
  if (index != -1) {
      running = false;
  }
}

void ActionScheduler::stop() {
    if (index != -1) {
        actions[index] = NULL;
        index = -1;
        running = false;
    }
}

void ActionScheduler::update() {
    if (running && millis() > nextTick) {
        unsigned long nextInterval = run();
        if (nextInterval != 0) {
            interval = nextInterval;
        }
        nextTick += interval;
    }
}

void ActionScheduler::updateActions() {
    for (int i = 0; i < maxActions; i++) {
        if (actions[i] != NULL) {
            actions[i]->update();
        }
    }
}

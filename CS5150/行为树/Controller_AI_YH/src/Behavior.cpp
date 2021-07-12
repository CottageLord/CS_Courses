#include "Behavior.h"

/*	This approach is a bit slower, since you must use conditional branches every tick(). 
	Most composite behaviors could handle this more efficiently since they process the return statuses anyway. 
	However, having such a wrapper function avoids many beginner mistakes
*/
Status Behavior::tick() {
	if (m_eStatus != Status::BH_RUNNING) onInitialize();
	m_eStatus = update();
	if (m_eStatus != Status::BH_RUNNING) onTerminate(m_eStatus);
	return m_eStatus;
}

Status Repeat::update() {
    for (;;) {
        m_pChild->tick();
        if (m_pChild->getStatus() == Status::BH_RUNNING) break;
        if (m_pChild->getStatus() == Status::BH_FAILURE) return Status::BH_FAILURE;
        if (++m_iCounter == m_iLimit) return Status::BH_SUCCESS;
        m_pChild->resetStatus();
    }
    return Status::BH_INVALID;
}
// see if all children could succeed
Status Sequence::update() {
    // Keep going until a child behavior says it's running.
    for (;;) {
        Status s = (*m_CurrentChild)->tick();
        // If the child fails, or keeps running, do the same.
        if (s != Status::BH_SUCCESS) return s;
        // Hit the end of the array, job done!
        if (++m_CurrentChild == m_Children.end()) return Status::BH_SUCCESS;
    }
}

// see if any child could succeed
Status Selector::update()
{
    // Keep going until a child behavior says its running.
    for (;;) {
        Status s = (*m_Current)->tick();
        // If the child succeeds, or keeps running, do the same.
        if (s != Status::BH_FAILURE) return s;
        // Hit the end of the array, it didn't end well...
        if (++m_Current == m_Children.end()) return Status::BH_FAILURE;
    }
}

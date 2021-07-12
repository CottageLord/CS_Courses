#pragma once
#include "Config.h"
#include <vector>

typedef void (*callback_function)(void); // type for conciseness

class Behavior {
protected:
	/* API identical to previous code listing. */
public:
	Behavior() : m_eStatus(Status::BH_INVALID) {}
	virtual ~Behavior() {}
    virtual void onInitialize() {}
    virtual void onTerminate(Status) {}
	virtual Status update() = 0;
	virtual Status tick();
    Status getStatus() const { return m_eStatus; }
    void resetStatus() { m_eStatus = Status::BH_INVALID; }
	/*... */
private:
	Status m_eStatus;
};

class Decorator : public Behavior {
protected:
	Behavior* m_pChild;
public:
	Decorator(Behavior* child) : m_pChild(child) {}
};

class Repeat : public Decorator {
public:
    Repeat(Behavior* child) : Decorator(child) {}
    void setCount(int count) { m_iLimit = count; }
    void onInitialize() { m_iCounter = 0; }
    Status update() override;
protected:
    int m_iLimit;
    int m_iCounter;
};


class Composite : public Behavior
{
public:
    void addChild(Behavior* child) { m_Children.push_back(child); }
    void removeChild(Behavior*);
    void clearChildren();
protected:
    typedef std::vector<Behavior*> Behaviors;
    Behaviors m_Children;
};

class Sequence : public Composite
{
protected:
    virtual ~Sequence() { }
    virtual void onInitialize() { m_CurrentChild = m_Children.begin(); }
    virtual Status update() override;
    Behaviors::iterator m_CurrentChild;
};


class Selector : public Composite
{
protected:
    virtual ~Selector() {}
    virtual void onInitialize() { m_Current = m_Children.begin(); }
    virtual Status update() override;
    Behaviors::iterator m_Current;
};

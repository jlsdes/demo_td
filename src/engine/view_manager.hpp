#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "view_object.hpp"
#include "utils/manager.hpp"

#include <memory>
#include <queue>


/** Abstract base class as an interface for queue implementations. */
class RenderQueue {
public:
    virtual ~RenderQueue();

    virtual void push( ViewObject const & object ) = 0;
    void push( ViewObject const * object );
    virtual ViewObject const & pop() = 0;
    virtual bool empty() = 0;
};

/** The most basic of queues: FIFO queue. */
class NaiveRenderQueue : public RenderQueue {
public:
    ~NaiveRenderQueue() override;

    void push( ViewObject const & object ) override;
    ViewObject const & pop() override;
    bool empty() override;

private:
    std::queue<ViewObject const *> m_objects;
};

/** */
class ViewManager : public Manager<ViewObject> {
public:
    explicit ViewManager( std::unique_ptr<RenderQueue> && queue = nullptr );
    ~ViewManager() override;

    void draw() const;

private:
    std::unique_ptr<RenderQueue> m_queue;
};


#endif //DEMO_TD_RENDERER_HPP

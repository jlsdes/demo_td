#ifndef DEMO_TD_RENDERER_HPP
#define DEMO_TD_RENDERER_HPP

#include "render_object.hpp"

#include <memory>
#include <queue>
#include <vector>


class Shader;

/** Abstract base class as an interface for queue implementations. */
class RenderQueue {
public:
    virtual ~RenderQueue();

    virtual void push( RenderObject const & object ) = 0;
    void push( RenderObject const * object );
    virtual RenderObject const & pop() = 0;
    virtual bool empty() = 0;
};

/** The most basic of queues: FIFO queue. */
class NaiveRenderQueue : public RenderQueue {
public:
    ~NaiveRenderQueue() override;

    void push( RenderObject const & object ) override;
    RenderObject const & pop() override;
    bool empty() override;

private:
    std::queue<RenderObject const *> m_objects;
};

/** */
class Renderer {
public:
    explicit Renderer( std::unique_ptr<RenderQueue> && queue = nullptr );
    ~Renderer();

    void register_object( RenderObject const & object );
    void unregister_object( RenderObject const & object );

    void draw() const;

private:
    std::vector<RenderObject const *> m_objects;
    std::unique_ptr<RenderQueue> m_queue;
};


#endif //DEMO_TD_RENDERER_HPP

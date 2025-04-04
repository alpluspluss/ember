#include <mutex>
#include <thread>
#include <queue>
#include <unordered_map>
#include <sparkle/ir/passes/manager.hpp>

namespace sprk
{
    class ThreadPool
    {
    public:
        explicit ThreadPool(size_t n)
        {
            for (size_t i = 0; i < n; ++i)
            {
                workers.emplace_back([this]()
                {
                    while (true)
                    {
                        std::function<void()> task;
                        /* lock scope */
                        {
                            std::unique_lock lock(queue_mutex);
                            condition.wait(lock, [this]
                            {
                                return !tasks.empty() || stop;
                            });

                            if (stop && tasks.empty())
                                return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        void enqueue(std::function<void()> task)
        {
            {
                std::lock_guard lock(queue_mutex);
                tasks.push(std::move(task));
            }
            condition.notify_one();
        }

        ~ThreadPool()
        {
            {
                std::lock_guard lock(queue_mutex);
                stop = true;
            }
            condition.notify_all();

            for (std::thread& worker : workers)
                worker.join();
        }

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;
    };

    std::vector<Pass> pass_config = {
        {
            /* AA */
            .deps = {},
            .id = PassId::ALIAS_ANALYSIS,
            .flags = PassFlags::NONE, /* mostly a prerequisite analysis; not parallel */
            .type = PassType::LOCAL /* region-only */
        },
        {
            /* DCE */
            .deps = {},
            .id = PassId::DEADCODE_ELIMINATION,
            .flags = PassFlags::HORIZONTAL_PARALLELISM | PassFlags::VERTICAL_PARALLELISM,
        },
        {
            /* IPA */
            .deps = {},
            .id = PassId::INTERPROCEDURAL_ANALYSIS,
            .flags = PassFlags::HORIZONTAL_PARALLELISM, /* can analyze functions in parallel */
            .type = PassType::INTER
        },
        {
            /* IPO */
            .deps = { PassId::INTERPROCEDURAL_ANALYSIS }, /* depends on IPA */
            .id = PassId::INTERPROCEDURAL_OPTIMIZATION,
            .flags = PassFlags::HORIZONTAL_PARALLELISM, 
            .type = PassType::INTER
        },
        {
            /* PRE */
            .deps = { PassId::ALIAS_ANALYSIS }, /* need alias info to avoid incorrect elimination */
            .id = PassId::PARTIAL_REDUNDANCY,
            .flags = PassFlags::HORIZONTAL_PARALLELISM, 
            .type = PassType::LOCAL
        }
    };

    void PassManager::run()
    {
        std::unordered_map<PassId, int> in_degs;
        std::unordered_map<PassId, std::vector<PassId>> adj_list;
        std::unordered_map<PassId, const Pass*> pass_map;

        /* build dependency graph */
        for (PassId id : passes)
        {
            const Pass* pass = nullptr;
            for (const auto& p : pass_config)
            {
                if (p.id == id)
                {
                    pass = &p;
                    break;
                }
            }

            if (!pass) /* unknown */
                continue;
            
            in_degs[pass->id] = pass->deps.size();
            pass_map[pass->id] = pass;
            for (auto dep : pass->deps)
                adj_list[dep].push_back(pass->id);
        }

        /* worklist for passes that is runnable */
        std::queue<PassId> worklist;
        for (const auto& [id, degree] : in_degs)
        {
            if (degree == 0)
                worklist.push(id);
        }

        /* execute passes in parallel */
        ThreadPool tp(std::thread::hardware_concurrency());
        std::mutex mtx;
        std::condition_variable cv;
        std::unordered_map<PassId, bool> completed;
        int remaining_tasks = worklist.size();

        while (!worklist.empty())
        {
            PassId id = worklist.front();
            worklist.pop();
            
            tp.enqueue([this, id, &mtx, &adj_list, &in_degs, &worklist, &completed, &cv, &remaining_tasks]() 
            {
                run_pass(id);
                
                std::lock_guard lock(mtx);
                completed[id] = true;
                
                for (auto next : adj_list[id])
                {
                    in_degs[next]--;
                    if (in_degs[next] == 0)
                    {
                        worklist.push(next);
                        remaining_tasks++;
                    }
                }
                
                remaining_tasks--;
                if (remaining_tasks == 0)
                    cv.notify_all();
            });
        }
        
        std::unique_lock lock(mtx);
        cv.wait(lock, [&remaining_tasks]() { return remaining_tasks == 0; });
    }

    void PassManager::run_pass(PassId id)
    {

    }
}

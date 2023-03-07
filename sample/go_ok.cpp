#include <workflow/WFHttpServer.h>

#include <chrono>
#include <thread>
#include <unistd.h>
#include <json.hpp>

using json = nlohmann::json;

int cnt = 0;

void task1()
{
    printf("in go func 1 \n");
    cnt++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("go func1 done!\n");
}

void task2()
{
    printf("in go func 2\n");
    cnt++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("go func2 done!\n");
}

void task3()
{
    printf("in go func 3\n");
    cnt++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("go func3 done!\n");
}

void task4()
{
    printf("in go func 4\n");
    cnt++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("go func4 done!\n");
}

// int main()
// {
//     WFGoTask *go_task = WFTaskFactory::create_go_task("go", Factorial);
//     go_task->start();

//     getchar();
//     return 0;
// }

#include <workflow/Workflow.h>
#include <workflow/WFTaskFactory.h>

// int main()
// {
//     ParallelWork *pwork = Workflow::create_parallel_work(nullptr);

//     WFGoTask *go_task1 = WFTaskFactory::create_go_task("go",  task1);
//     SeriesWork *series1 = Workflow::create_series_work(go_task1, nullptr);
//     pwork->add_series(series1);


//     WFGoTask *go_task2 = WFTaskFactory::create_go_task("go",  task2);
//     SeriesWork *series2 = Workflow::create_series_work(go_task2, nullptr);
//     pwork->add_series(series2);

//     WFGoTask *go_task3 = WFTaskFactory::create_go_task("go",  task3);
//     SeriesWork *series3 = Workflow::create_series_work(go_task3, nullptr);
//     pwork->add_series(series3);

//     pwork->start();
//     getchar();
//     return 0;
// }


#include <atomic>

#include "workflow/WFTaskFactory.h"
#include "workflow/WFFacilities.h"


static SubTask *create_task1()
{
	return WFTaskFactory::create_go_task("go",  task1);
}

static SubTask *create_task2()
{
	return WFTaskFactory::create_go_task("go",  task2);
}

static SubTask *create_task3()
{
	return WFTaskFactory::create_go_task("go",  task3);
}

static SubTask *create_task4()
{
	return WFTaskFactory::create_go_task("go",  task4);
}

using MyFactory = WFThreadTaskFactory<json *, json *>;
using MyTask = WFThreadTask<json *, json *>;


// int main(){

//     // int cnt = 0;

// 	auto graph = WFTaskFactory::create_graph_task(nullptr);

// 	auto& a = graph->create_graph_node(create_task1());
// 	auto& b = graph->create_graph_node(create_task2());
//     auto& c = graph->create_graph_node(create_task3());
//     auto& d = graph->create_graph_node(create_task4());

// 	c-->a;
//     c-->b;
//     c-->d;
//     b-->a;
//     d-->a;

// 	graph->start();

//     sleep(10);
//     printf("cnt:%d\n", cnt);
// }

int main(){
    auto&& calc_multi = [](json **in, json **out) {
        printf("000 in:%p out:%p\n", *in , *out);
        auto &ture_in = (**in);
        auto &ture_out = (**out);
        printf("010\n");
		ture_out["m_res"] = ture_in["m_first"].get<int>() * ture_in["m_second"].get<int>();
        printf("111\n");
	};

    auto&& calc_plus = [](json **in, json **out) {
		(**out)["p_res"] = (**in)["m_res"].get<int>()  + 100;
        printf("222\n");
	};

	auto *task_m = MyFactory::create_thread_task("calc", std::move(calc_multi), [](MyTask *task) {
		auto state = task->get_state();
		if (state == WFT_STATE_SUCCESS)
		{
			auto *out = task->get_output();
            printf("output_m:%d\n", (**out)["m_res"].get<int>());
		}
	});

	auto *task_p = MyFactory::create_thread_task("calc", std::move(calc_plus), [](MyTask *task) {
		auto state = task->get_state();
		if (state == WFT_STATE_SUCCESS)
		{
			auto *out = task->get_output();
            printf("output_p:%d\n", (**out)["p_res"].get<int>());
		}
	});


    json **m_input = task_m->get_input();
    json **m_output = task_m->get_output();
    json **p_input = task_p->get_input();
    json **p_ouput = task_p->get_output();


	auto *series = Workflow::create_series_work(task_m, nullptr);

	series->push_back(task_p);
    for(int i = 0; i < 3; i++){
        json all;
        *m_input = &all;
        *m_output = *m_input;
        *p_input = *m_input;
        *p_ouput = *m_input;

        printf("%p %p\n", *m_input, *m_output);

        (**m_input)["m_first"] = i;
        (**m_input)["m_second"] = i;

        series->start();
        sleep(3);
    }

    getchar();

    return 0;
}

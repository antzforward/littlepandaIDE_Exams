#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <tbb/parallel_reduce.h>

const size_t N = 100000000;  // 1亿个元素

int main() {
	std::vector<float> data(N);
	
	// 初始化数据
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	
	// 并行初始化
	tbb::parallel_for(tbb::blocked_range<size_t>(0, N),
					  [&](const tbb::blocked_range<size_t>& r) {
						  for (size_t i = r.begin(); i < r.end(); ++i) {
							  data[i] = dis(gen);
						  }
					  });
	
	std::cout << "TBB并行归约测试" << std::endl;
	std::cout << "数据量: " << N << " 个浮点数" << std::endl;
	
	// 预热
	float warmup_result = tbb::parallel_reduce(
											   tbb::blocked_range<size_t>(0, N), 0.0f,
											   [&](const tbb::blocked_range<size_t>& r, float init) {
												   for (size_t i = r.begin(); i < r.end(); ++i) {
													   init += data[i];
												   }
												   return init;
											   },
											   [](float a, float b) { return a + b; }
											   );
	
	// 正式测试
	int iterations = 10;
	auto total_duration = std::chrono::milliseconds(0);
	float final_result = 0.0f;
	
	for (int iter = 0; iter < iterations; ++iter) {
		auto start = std::chrono::high_resolution_clock::now();
		
		final_result = tbb::parallel_reduce(
											tbb::blocked_range<size_t>(0, N), 0.0f,
											[&](const tbb::blocked_range<size_t>& r, float init) {
												for (size_t i = r.begin(); i < r.end(); ++i) {
													init += data[i];
												}
												return init;
											},
											[](float a, float b) { return a + b; }
											);
		
		auto end = std::chrono::high_resolution_clock::now();
		total_duration += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	}
	
	std::cout << "迭代次数: " << iterations << std::endl;
	std::cout << "总耗时: " << total_duration.count() << "ms" << std::endl;
	std::cout << "平均耗时: " << total_duration.count() / (double)iterations << "ms" << std::endl;
	std::cout << "归约结果: " << final_result << std::endl;
	std::cout << "期望值范围: " << N * 0.5f << " ± " << N * 0.05f << std::endl;
	
	return 0;
}

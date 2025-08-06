void parallel_quick_sort(std::vector<int>& data, ThreadPool& pool, int threshold) {
    if (data.size() <= 1)
        return;

    // Внутренняя рекурсивная функция
    std::function<void(std::vector<int>&)> quick_sort_recursive = [&](std::vector<int>& arr) {
        if (arr.size() <= 1)
            return;

        int pivot = arr[arr.size() / 2];
        std::vector<int> less, greater;

        for (int v : arr) {
            if (v < pivot)
                less.push_back(v);
            else if (v > pivot)
                greater.push_back(v);
        }

        if (arr.size() < threshold) {
            // Последовательная сортировка
            quick_sort_recursive(less);
            quick_sort_recursive(greater);
        } else {
            // Создаем родительскую задачу
            auto control = std::make_shared<TaskControl>(2);
            auto parent_future = control->promise->get_future();

            // Запускаем подзадачи
            pool.enqueue([less = std::move(less), control, &pool, threshold]() {
                quick_sort_recursive(less);
                if (--(*control->counter) == 0)
                    control->promise->set_value();
            });

            pool.enqueue([greater = std::move(greater), control, &pool, threshold]() {
                quick_sort_recursive(greater);
                if (--(*control->counter) == 0)
                    control->promise->set_value();
            });

            // Ждем завершения обеих подзадач
            parent_future.get();

            // Объединяем результаты
            arr.clear();
            arr.reserve(less.size() + greater.size());
            arr.insert(arr.end(), less.begin(), less.end());
            arr.insert(arr.end(), greater.begin(), greater.end());
        }
    };

    quick_sort_recursive(data);
}

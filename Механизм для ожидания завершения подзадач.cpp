// Создаем родительскую задачу с promise и shared_ptr счетчиком
struct TaskControl {
    std::shared_ptr<int> counter;
    std::shared_ptr<std::promise<void>> promise;

    TaskControl(int count) : counter(std::make_shared<int>(count)),
                             promise(std::make_shared<std::promise<void>>()) {}
};

class CanWalkAndLock {
public:
    void Setup();
    void Loop();

private:
    static void AliveLedTask(void* pvParameters);
    void AliveLedTask();
};
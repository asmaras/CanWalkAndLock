class CanWalkAndLock {
public:
    void Setup();
    void Loop();

private:
    static void AliveLedTask(void* pvParameters);
    void AliveLedTask();
    static void DiagnosticsLogTask(void* pvParameters);
    void DiagnosticsLogTask();

    static constexpr const char* _logTag = "CanWalkAndLock";
};
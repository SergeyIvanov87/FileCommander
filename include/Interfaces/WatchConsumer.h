#ifndef WATCH_CONSUMER_H
#define WATCH_CONSUMER_H

/* IResutConsumer
 * Used by Command for notify observer about execution process
 */
struct IResutConsumer
{
protected:
    mutable std::mutex m_descriptionMutex;
    std::stringstream desription;
public:
    IResutConsumer() = default;
    ~IResutConsumer() = default;
    template <class One>
    void notify(One &&one);

    template <class Last, class ...Args>
    void notify(Last &&last, Args &&...args);

    virtual void setFinalize() = 0;

    virtual void setInterrupt() {};
    virtual bool isInterrupted() const { return false; };
};

/* IResultProvider
 * Used by Observer for collect result of command execution
 */
struct IResultProvider
{
    virtual std::string getLastResult() = 0;
    virtual bool isFinalized() const = 0;
};

#endif //WATCH_CONSUMER_H

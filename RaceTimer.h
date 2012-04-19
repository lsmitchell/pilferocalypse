#include <ctime>
typedef float diff_type;

class RaceTimer {
public:
  RaceTimer(void);
  ~RaceTimer(void);

  void start();
  void delayedStart(diff_type delayNum);
  diff_type stop();
  void pause();
  diff_type getCurrentSeconds();

private:
  clock_t begin, end;
  diff_type elapsedMilli, elapsedSeconds, elapsedMinutes;
  int elapsedTicks;
  bool isPaused;
  bool isStopped;
  diff_type pauseVal;
};
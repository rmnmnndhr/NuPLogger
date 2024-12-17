## Einbinden des Loggers
1. Erstellen eines Loggerobjektes aus dem Singleton:
        **Logger &logger = Logger::getInstance("log.txt");**
2. Registrieren einer Task über derren Id
         **logger.registerSource("task3");**
3. Loggen eines Wertes.
         **logger.log("task3", "Task3 log entry.");**

### Pausieren

logger.suspend();

### Weiterlaufen lassen

logger.resume();
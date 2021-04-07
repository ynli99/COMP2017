#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 4097 // Max character 4096 not including \n or \0

// Check if place name is valid
// Colons in place name shouldn't be valid as it can lead to ambiguity
int checkNameValidity(char s[]) {
    char *p = &s[0];

    while (*p) {
        if (*p == ':') {
            return -1;
        }
        p++;
    }

    return 0;
}

int compareStrings(char s1[], char s2[]) {
    char *p1 = &s1[0];
    char *p2 = &s2[0];
    while (*p1 || *p2) {
        // Return -1 if two strings are different in length
        if ((*p1 && !*p2) || (!*p1 && *p2)) {
            return -1;
        } 
        // If one character is different return -1
        else if (*p1 != *p2) {
            return -1;
        }
        // If same character
        else if (*p1 == *p2) {
            p1++;
            p2++;
        }
    }

    // If sucessfully reach end of loop (same string)
    return 0;
}

int convertTimeFormat(int times[], char args[]) {
    int i = 0;
    int n = 0;
    while (*args && *args != '\n') {
        if(*args == ':') {
            n = 0;
            i++;
        } else {
            if (*args < 48 || *args > 58) {
                return -1;
            }
            // Calculate time (-48 converts ASCII to digits)
            n = n * 10 + (*args - 48); 
        }

        times[i] = n;
        args++;
    }

    // Time format wrong
    if (times[0] > 23 || times[1] > 59 || times[2] > 59) {
        return -1;
    }

    return 0;
}

void revertTimeFormat(int times[], char timeString[]) {
    // All +48 in ASCII to convert back
    for (int i = 0, j = 0; i < 3; i++) {
        if (j == 2 || j == 5) {
            timeString[j] = ':';
            j++;
        }

        if (times[i] < 10) {
            timeString[j] = '0';
            j++;
            timeString[j] = times[i] + 48;
            j++;
        } else {
            timeString[j] = times[i] / 10 + 48;
            j++;
            timeString[j] = times[i] % 10 + 48;
            j++;
        }
    }

    timeString[8] = '\0';
}

int processInput(char input[], char sou[], char des[], int tim[]) {
    int c = 0; // Colons for parts
    int totalColons = 0;
    char *i = &input[0];
    int j = 0;

    // No source station
    if (*i == ':') {
        return -1;
    }

    while (*i && *i != '\n') {
        // Too many colons
        if (totalColons > 4 || c > 2) {
            return -1;
        }

        if (*i == ':') {
            c++;
            totalColons++;
            j = 0;
        } else {
            c = 0;
            // Source station
            if (totalColons == 0) {
                sou[j] = *i;
                sou[j+1] = '\0';
                j++;
            } 
            // Destination station
            else if (totalColons == 2) {
                des[j] = *i;
                des[j+1] = '\0';
                j++;
            }
            // Time formatting
            else if (totalColons == 4) {
                // Check if length of time format is right
                int length = 0;
                char t[9];
                while (*i && *i != '\n') {
                    if (c > 2) {
                        return -1;
                    }
                    if (*i == ':') {
                        c++;
                    }

                    t[j] = *i;
                    j++;
                    length++;
                    i++;

                    if (length > 8) {
                        return -1;
                    }
                }

                if (length < 8) {
                    return -1;
                }

                t[8] = '\0'; // Set null terminator for end of string

                if (convertTimeFormat(tim, t) == -1) {
                    return -1;
                }

                break;
            }
        }
        i++;
    }

    // Too little colons
    if (totalColons < 4) {
        return -1;
    }

    return 0;
}

// Calculations from hours to seconds for easier comparison
int hourToSec(int ls[]) {
    return ls[0] * 3600 + ls[1] * 60 + ls[2];
} 

int main (int argc, char *argv[]) {
    // Check if correct number of command line arguments are supplied
    if (argc != 4) {
        fprintf(stderr, "Please provide <source> <destination> <time> as "
            "command line arguments\n");
        exit(1);
    }

    char buffer[BUFSIZE] = "";
    char *source = argv[1];
    char *destination = argv[2];

    if (checkNameValidity(source) == -1 ||
     checkNameValidity(destination) == -1) {
        fprintf(stderr, "Invalid place name\n");
        exit(2);
    }

    int time[3];
    if (convertTimeFormat(time, argv[3]) == -1) {
		fprintf(stderr, "Wrong time format\n");
		exit(3);
	}

    int arrTimeInSeconds = hourToSec(time);
    int closestNext[] = {-1, -1, -1};
    
    while (fgets(buffer, BUFSIZE, stdin)) {
        // If input size larger than 4096
        if (buffer[4096] != 0 && buffer[4096] != '\n') {
            continue;
        } else {
            // Max source length = 4096-13 (if dest length = 1), vice versa
            // Get info from file
            char tsource[4083];
            char tdest[4083];
            int ttime[3];

            // Invalid time format
            if (processInput(buffer, tsource, tdest, ttime) == -1) {
                continue;
            } 

            // If current source or destination doesn't match target
            if (compareStrings(source, tsource) == -1 || 
            compareStrings(destination, tdest) == -1) {
                continue;
            } else {
                int timetableInSeconds = hourToSec(ttime);
                if (timetableInSeconds < arrTimeInSeconds) {
                    continue;
                } else {
                    if (closestNext[0] == -1) {
                        closestNext[0] = ttime[0];
                        closestNext[1] = ttime[1];
                        closestNext[2] = ttime[2];
                    } else {
                        // If current schedule is closer to arrival time than 
                        // previous closest
                        if (timetableInSeconds < hourToSec(closestNext)) {
                            closestNext[0] = ttime[0];
                            closestNext[1] = ttime[1];
                            closestNext[2] = ttime[2];
                        }
                    }
                }
            }
        }
    }

    if (buffer[0] == '\0') {
        fprintf(stderr, "No timetable to process\n");
    } else {
        if (closestNext[0] == -1) {
            printf("No suitable trains can be found\n");
        } else {
            char timeStr[9];
            revertTimeFormat(closestNext, timeStr);
            printf("The next train to %s from %s departs at %s\n", destination,
             source, timeStr);
        }
    }

    return 0;
}


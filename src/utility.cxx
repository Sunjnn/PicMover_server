#include "utility.hxx"

#include <limits>
#include <vector>

#include <qstring.h>

using std::numeric_limits;
using std::vector;

QString generate_name() {
    const vector<QString> adjectives = {"quick",  "lazy",  "happy", "sad",    "brave",
                                        "clever", "witty", "calm",  "bright", "gentle"};
    const vector<QString> nouns = {"fox", "dog", "cat", "mouse", "lion", "tiger", "bear", "wolf", "eagle", "shark"};

    int intMax = numeric_limits<int>::max();
    int adjectiveIndex = generate_random_number(0, static_cast<int>(adjectives.size() - 1));
    int nounIndex = generate_random_number(0, static_cast<int>(nouns.size() - 1));
    int number = generate_random_number(1, 9999);

    QString name = adjectives[adjectiveIndex] + "_" + nouns[nounIndex] + "_" + QString::number(number);
    return name;
}

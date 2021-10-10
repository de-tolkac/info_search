#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <regex>
#include <vector>
#include <cstdlib>
#include <map>
#include <cmath>
#include <algorithm>


bool filesExist() {
    bool result = true;

    if (!std::filesystem::exists("facts.txt")) {
        result = false;
        std::cout << "Missed file: facts.txt\n";
    }

    if (!std::filesystem::exists("./articles/article1.txt")) {
        result = false;
        std::cout << "Missed file: article1.txt\n";
    }

    if (!std::filesystem::exists("./articles/article2.txt")) {
        result = false;
        std::cout << "Missed file: article2.txt\n";
    }

    if (!std::filesystem::exists("./articles/article3.txt")) {
        result = false;
        std::cout << "Missed file: article3.txt\n";
    }

    if (!std::filesystem::exists("mystem")) {
        result = false;
        std::cout << "Missed mystem module\n";
    }

    return result;
}

// ./mystem -clsd  ./articles/article1.txt ./articles/test1.txt
bool lemmatize() {
    if (system("./mystem -clsd  ./articles/article1.txt ./mystem_output/article1.txt") < 0) {
        std::cout << "Mystem error for file \'article1.txt\'\n";
        return false;
    }

    if (system("./mystem -clsd  ./articles/article2.txt ./mystem_output/article2.txt") < 0) {
        std::cout << "Mystem error for file \'article2.txt\'\n";
        return false;
    }

    if (system("./mystem -clsd  ./articles/article3.txt ./mystem_output/article3.txt") < 0) {
        std::cout << "Mystem error for file \'article3.txt\'\n";
        return false;
    }


    return true;
}

bool lemmatize(std::vector<std::string>& files) {
    for (const auto& f : files) {
        std::string cmd = "./mystem -clsd ./articles/";
        cmd += f;
        cmd += " ./mystem_output/";
        cmd += f;
        if (system(cmd.c_str()) < 0) {
            std::cout << "Mystem error for file" << f << "\n";
            return false;
        }
    }
    
    return true;
}

void parseArticle(std::vector<std::vector<std::string>>& sentences, std::string fileName) {
    std::string path = "./mystem_output/";
    path += fileName;

    std::ifstream file(path);
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    sentences.push_back({});
    std::string word;
    for (auto it = text.begin(); it != text.end(); ++it) {
        if (*it == '{') {
            ++it;
            while (*it != '}') {
                if (*it != '?') {
                    word.push_back(*it);
                }
                ++it;
            }
            if (word == "\\s") {
                sentences.push_back({});
            } else {
                sentences[sentences.size() - 1].push_back(word);
            }
            word.clear();
        }
    }
}

void printArticle(std::vector<std::vector<std::string>>& sentences, std::string fileName) {
    std::ofstream file(fileName);
    for (const auto& s : sentences) {
        for (const auto& w : s) {
            file << w << ' ';
        }
        file << "\n\n\n";
    }
    file.close();
}

void normalize(std::map<std::string, double>& vec, std::map<std::string, int>& countedWords, double totalSentences) {
    double norm = 0;
    for (auto& w : vec) {
        w.second *= (totalSentences / (double)countedWords[w.first]);
        norm += w.second * w.second;
    }

    norm = std::sqrt(norm);
    for (auto& w : vec) {
        w.second /= norm;
    }
}

double dotProduct(std::map<std::string, double>& lhs, std::map<std::string, double>& rhs) {
    double result = 0;
    for (const auto& w : lhs) {
        if (rhs.find(w.first) != rhs.end()) {
            result += w.second * rhs[w.first];
        }
    }

    return result;
}

void calculate(std::vector<std::string>& fact, std::vector<std::vector<std::string>>& article, std::string fileName) {
    std::map<std::string, int> countedWords;
    std::map<std::string, double> fact_vec;

    for (const auto& w : fact) {
        if (fact_vec.find(w) != fact_vec.end()) {
            ++fact_vec[w];
        } else {
            fact_vec[w] = 1;
            if (countedWords.find(w) != countedWords.end()) {
                    ++countedWords[w];
                } else {
                    countedWords[w] = 1;
                }
        }
    }

    std::vector<std::map<std::string, double>> sentences;
    
    for (size_t i = 0; i < article.size(); ++i) {
        sentences.push_back({});
        for (const auto& w : article[i]) {
            if (sentences[i].find(w) != sentences[i].end()) {
                ++sentences[i][w];
            } else {
                sentences[i][w] = 1;
                if (countedWords.find(w) != countedWords.end()) {
                    ++countedWords[w];
                } else {
                    countedWords[w] = 1;
                }
            }
        }
    }

    double totalSentences = sentences.size();
    for (auto& s : sentences) {
        normalize(s, countedWords, totalSentences);
    }
    
    normalize(fact_vec, countedWords, totalSentences);

    std::vector<std::pair<double, size_t>> results;
    for (size_t i = 0; i < sentences.size(); ++i) {
        results.push_back({dotProduct(fact_vec, sentences[i]), i});
    }
    std::sort(results.begin(), results.end(), [](auto& lhs, auto& rhs) {
        return lhs.first > rhs.first;
    });

    std::string path = "./output/";
    path += fileName;

    std::ofstream output(path);
    for (const auto& w : fact) {
        output << w << ' ';
    }
    output << "\n\n";
    for (size_t i = 0; i < results.size(); ++i) {
        output << i + 1 << " (" << results[i].first << "): ";
        for (const auto& w : article[results[i].second]) {
            output << w << ' ';
        }
        output << '\n';
    }
    output.close();
}

void countWords(std::vector<std::vector<std::vector<std::string>>>& articles, std::vector<std::map<std::string, int>>& countedWords) {
    for (const auto& a : articles) {
       for (const auto& sentence : a) {
           for (const auto& w : sentence) {

           }
       }
    }
}

int main() {
    const size_t articlesNumber = 3;
    
    std::vector<std::string> articleFiles = {"article1.txt", "article2.txt", "article3.txt"};
    lemmatize(articleFiles);

    std::vector<std::string> facts[articlesNumber];
    facts[0] = {"немецкий", "монахиня", "побеждать", "проказа", "в", "пакистан"};
    facts[1]= {"с", "медов", "шифрование", "любой", "ключ", "казаться", "подходящий"};
    facts[2] = {"российский", "промышленный", "турист", "часто", "все", "посещать", "завод", "пищевой", "промышленность"};
    
    std::vector<std::vector<std::vector<std::string>>> articles;
    articles.resize(articlesNumber);
    for (size_t i = 0; i < articlesNumber; ++i) {
        parseArticle(articles[i], articleFiles[i]);
    }

    for (size_t i = 0; i < articlesNumber; ++i) {
        std::string fileName = "article";
        fileName += std::to_string(i + 1);
        fileName += ".txt";
        calculate(facts[i], articles[i], fileName); 
    }
    return 0;
}
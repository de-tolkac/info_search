#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <map>
#include <cmath>
#include <algorithm>


struct Document {
    std::map<std::string, double> map;
    std::vector<std::string>* pointer;
};

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

void normalize(std::map<std::string, double>& vec, std::map<std::string, int>& countedWords, double totalDocuments) {
    double norm = 0;
    for (auto& w : vec) {
        if (countedWords[w.first] != 0) {
            w.second *= (totalDocuments / (double)countedWords[w.first]);
            norm += w.second * w.second;
        }
    }

    norm = std::sqrt(norm);
    if (norm != 0) {
        for (auto& w : vec) {
            w.second /= norm;
        }
    }
}

void normalizeDocuments(std::vector<Document>& documents, std::map<std::string, int>& countedWords, double totalDocuments) {
    for (auto& d : documents) {
        normalize(d.map, countedWords, totalDocuments);
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

void countWords(std::vector<Document>& documents, std::map<std::string, int>& countedWords) {
    for (const auto& d : documents) {
        for (const auto& w : d.map) {
            if (countedWords.find(w.first) != countedWords.end()) {
                ++countedWords[w.first];
            } else {
                countedWords[w.first] = 1;
            }
        }
    }
}

void countWordsInDocuments(std::vector<std::vector<std::vector<std::string>>>& articles, std::vector<Document>& documents) {
    for (auto& article : articles) {
        for (auto& sentence : article) {
            Document doc;
            doc.pointer = &sentence;
            for (const auto& word : sentence) {
                if (doc.map.find(word) != doc.map.end()) {
                    ++doc.map[word];
                } else {
                    doc.map[word] = 1;
                }
            }
            documents.push_back(doc);
        }
    }
}

void calculate(std::vector<std::string>& fact_raw, std::vector<Document>& documents, std::map<std::string, int>& countedWords, double totalDocuments, size_t fact_number) {
    std::vector<std::pair<double, size_t>> results;
    std::map<std::string, double> fact;

    for (const auto& w : fact_raw) {
        if (fact.find(w) != fact.end()) {
            ++fact[w];
        } else {
            fact[w] = 1;
        }
    }

    normalize(fact, countedWords, totalDocuments);

    for (size_t i = 0; i < documents.size(); ++i) {
        results.push_back({dotProduct(fact, documents[i].map), i});
    }
    std::sort(results.begin(), results.end(), [](auto& lhs, auto& rhs) {
        return lhs.first > rhs.first;
    });

    std::string path = "./output/";
    path += std::to_string(fact_number);
    path += ".txt";

    std::ofstream output(path);
    for (const auto& w : fact_raw) {
        output << w << ' ';
    }
    
    output << "\n\n";
    for (size_t i = 0; i < results.size(); ++i) {
        output << i + 1 << " (" << results[i].first << "): ";
        for (auto& w : (*documents[results[i].second].pointer)) {
            output << w << " ";
        }
        output << '\n';
    }
    output.close();
}

int main() {
    std::ofstream ff("test.txt");
    
    std::vector<std::string> articlesFiles = {"article1.txt", "article2.txt", "article3.txt", "article4.txt", "article5.txt", "article6.txt"};
    const size_t articlesNumber = articlesFiles.size();
    lemmatize(articlesFiles);

    std::vector<std::string> facts[3];
    facts[0] = {"немецкий", "монахиня", "побеждать", "проказа", "в", "пакистан"};
    facts[1]= {"с", "медов", "шифрование", "любой", "ключ", "казаться", "подходящий"};
    facts[2] = {"российский", "промышленный", "турист", "часто", "все", "посещать", "завод", "пищевой", "промышленность"};
    const size_t factsNumber = 3;
    
    std::vector<std::vector<std::vector<std::string>>> articles;
    articles.resize(articlesNumber);

    for (size_t i = 0; i < articlesNumber; ++i) {
        parseArticle(articles[i], articlesFiles[i]);
    }

    std::vector<Document> documents;
    countWordsInDocuments(articles, documents);

    std::map<std::string, int> countedWords;
    countWords(documents, countedWords);

    const size_t totalDocuments = documents.size();
    normalizeDocuments(documents, countedWords, totalDocuments);
    
    for (size_t i = 0; i < factsNumber; ++i) {
        calculate(facts[i], documents, countedWords, totalDocuments, i + 1);
    }

    return 0;
}
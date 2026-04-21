#include <bits/stdc++.h>
using namespace std;

static inline bool is_integer_line(const string &s) {
    size_t i = 0, n = s.size();
    while (i < n && isspace(static_cast<unsigned char>(s[i]))) ++i;
    if (i < n && (s[i] == '+' || s[i] == '-')) ++i;
    bool has_digit = false;
    while (i < n && isdigit(static_cast<unsigned char>(s[i]))) { ++i; has_digit = true; }
    while (i < n && isspace(static_cast<unsigned char>(s[i]))) ++i;
    return has_digit && i == n;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<string> lines;
    string line;
    while (getline(cin, line)) {
        if (!line.empty()) lines.push_back(line);
    }

    vector<string> commands;
    if (!lines.empty() && is_integer_line(lines[0])) {
        // If the first line is an integer N, take up to N following lines as commands
        long long N = 0;
        try {
            N = stoll(lines[0]);
        } catch (...) {
            N = 0;
        }
        for (size_t i = 1; i < lines.size() && (long long)commands.size() < N; ++i) {
            commands.push_back(lines[i]);
        }
        // If fewer lines than N are provided, just process what we have
    } else {
        commands = lines; // Process all lines as commands until EOF
    }

    deque<long long> dq;
    ostringstream out;

    for (const string &cmdline : commands) {
        if (cmdline.empty()) continue;
        // Tokenize
        string op; long long x = 0;
        {
            // parse first token
            size_t i = 0, n = cmdline.size();
            while (i < n && isspace((unsigned char)cmdline[i])) ++i;
            size_t start = i;
            while (i < n && !isspace((unsigned char)cmdline[i])) ++i;
            op = cmdline.substr(start, i - start);
            while (i < n && isspace((unsigned char)cmdline[i])) ++i;
            if (i < n) {
                // parse potential argument
                // Accept +/-, digits
                size_t j = i;
                bool neg = false;
                if (j < n && (cmdline[j] == '+' || cmdline[j] == '-')) { neg = (cmdline[j] == '-'); ++j; }
                long long val = 0; bool any = false;
                while (j < n && isdigit((unsigned char)cmdline[j])) { any = true; val = val * 10 + (cmdline[j]-'0'); ++j; }
                if (any) x = neg ? -val : val; // else leave x = 0
            }
        }

        if (op == "push_front") {
            dq.push_front(x);
        } else if (op == "push_back") {
            dq.push_back(x);
        } else if (op == "pop_front") {
            if (dq.empty()) out << -1 << '\n';
            else { out << dq.front() << '\n'; dq.pop_front(); }
        } else if (op == "pop_back") {
            if (dq.empty()) out << -1 << '\n';
            else { out << dq.back() << '\n'; dq.pop_back(); }
        } else if (op == "size") {
            out << dq.size() << '\n';
        } else if (op == "empty") {
            out << (dq.empty() ? 1 : 0) << '\n';
        } else if (op == "front") {
            if (dq.empty()) out << -1 << '\n';
            else out << dq.front() << '\n';
        } else if (op == "back") {
            if (dq.empty()) out << -1 << '\n';
            else out << dq.back() << '\n';
        } else if (op == "clear") {
            dq.clear();
        } else if (op == "print") {
            for (size_t i = 0; i < dq.size(); ++i) {
                if (i) out << ' ';
                out << dq[i];
            }
            out << '\n';
        } else {
            // Unknown command: ignore silently to avoid spurious output
        }
    }

    cout << out.str();
    return 0;
}

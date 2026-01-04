#include <unordered_map>
#include <unordered_set>

#ifndef __PEERS_CPP__

extern void update_peers(std::string addr, std::string topic);
extern void cleanup_peers(void);
extern void print_peers_stats(void);

extern std::unordered_map<std::string, int> topicCounts;

#endif

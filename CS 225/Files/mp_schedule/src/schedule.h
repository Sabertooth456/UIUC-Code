/**
 * @file schedule.h
 * Exam scheduling using graph coloring
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <set>

typedef std::vector<std::vector<std::string> > V2D; 

struct GraphNode {
    GraphNode();
    GraphNode(std::string name);
    int color = -1;
    std::string name = "";
    std::vector<GraphNode*> neighbors;
};

// This is a terribly implemented Graph and I am okay with that
class Graph {
    public:
    Graph();
    Graph(const V2D & courses);
    ~Graph();
    /**
     * Gets the neighbors of the idx'th node
    */
    GraphNode* getNode(int idx);
    void resetColor();
    unsigned size();
    void greedyColor(GraphNode* cur_node);
    unsigned maximum_colors = 0;
    private:
    std::vector<GraphNode*> nodes;
    void addNodes(const V2D & courses);
    void addNeighbors(V2D & courses);
    void connectNodes(int idx_1, int idx_2);
};

/**
 * Given a filename to a CSV-formatted text file, create a 2D vector of strings where each row
 * in the text file is a row in the V2D and each comma-separated value is stripped of whitespace
 * and stored as its own string. You must preserve the order from the input file.
 *  
 * @param filename The filename of a CSV-formatted text file. 
 * @return The vector of vectors of strings imported from filename
 */
V2D file_to_V2D(const std::string & filename);

/**
 * Given a course roster and a list of students and their courses, 
 * perform data correction and return a course roster of valid students (and only non-empty courses).
 * 
 * A 'valid student' is a student who is both in the course roster and the student's own listing contains the course
 * A course which has no students (or all students have been removed for not being valid) should be removed
 * 
 * @param course A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param student A 2D vector of strings where each row is a student ID followed by the courses they are taking
 * @return A 2D vector of strings where each row is a course followed by all the students in the course
 */
V2D clean(const V2D & cv, const V2D & sv);

/**
 * Helper method that checks if student has registered for and is assigned to a course
*/
bool valid_registration(std::string course, std::string student, const V2D & cv, const V2D & students);

/**
 * Given a collection of courses and a list of available times, create a valid scheduling (if possible).
 * 
 * A 'valid schedule' should assign each course to a timeslot in such a way that there are no conflicts for exams
 * In other words, two courses who share a student should not share an exam time.
 * Your solution should try to minimize the total number of timeslots but should not exceed the timeslots given.
 * 
 * If a valid solution cannot be formed after <XXX: How many iterations?> iterations of graph coloring, return a V2D with one row
 * with the string '-1' as the only value. 
 * 
 * @param courses A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param timeslots A vector of strings giving the total number of unique timeslots
 * @return A 2D vector which each row is a timeslot with all the courses scheduled for that time
 */
V2D schedule(const V2D &courses, const std::vector<std::string> &timeslots);
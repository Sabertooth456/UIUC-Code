/**
 * @file schedule.cpp
 * Exam scheduling using graph coloring
 */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <map>
#include <unordered_map>

#include "schedule.h"
#include "utils.h"
#include <algorithm>

GraphNode::GraphNode() {}

GraphNode::GraphNode(std::string name) : name(name) {}

Graph::~Graph() {
    for (unsigned idx = 0; idx < nodes.size(); idx++) {
        delete nodes[idx];
    }
    nodes.clear();
}

GraphNode* Graph::getNode(int idx) {
    return nodes[idx];
}

void Graph::resetColor() {
    for (GraphNode* node : nodes) {
        node->color = -1;
    }
    maximum_colors = 0;
}

unsigned Graph::size() {
    return nodes.size();
}

void Graph::greedyColor(GraphNode* cur_node) {
    if (cur_node->color != -1) {
        return;
    }
    std::set<int> invalid_colors;
    for (GraphNode* neighbor : cur_node->neighbors) {
        invalid_colors.insert(neighbor->color);
    }
    unsigned valid_color = 0;
    while(std::find(invalid_colors.begin(), invalid_colors.end(), valid_color) != invalid_colors.end()) {
        valid_color++;
    }
    cur_node->color = valid_color;
    if (valid_color + 1 > maximum_colors) {
        maximum_colors = valid_color + 1;
    }
    for (GraphNode* neighbor : cur_node->neighbors) {
        greedyColor(neighbor);
    }
}

Graph::Graph() {}

Graph::Graph(const V2D & courses) {
    V2D copy_of_courses = courses;
    addNodes(copy_of_courses);
    addNeighbors(copy_of_courses);
}

void Graph::addNeighbors(V2D & courses) {
    for (unsigned course_idx = 0; course_idx < courses.size(); course_idx++) { // For each course
        for (unsigned student_idx = 1; student_idx < courses[course_idx].size(); student_idx++) { // For each student in the course
            std::string student = courses[course_idx][student_idx];
            for (unsigned overlap_idx = course_idx + 1; overlap_idx < courses.size(); overlap_idx++) {
                // If they are taking another course (so the two courses would overlap)
                if (std::find(courses[overlap_idx].begin(), courses[overlap_idx].end(), student) != courses[overlap_idx].end()) {
                    // Connect the two course nodes
                    connectNodes(course_idx, overlap_idx);
                }
            }
        }
    }
}

void Graph::connectNodes(int idx_1, int idx_2) {
    GraphNode* node_1 = nodes[idx_1];
    GraphNode* node_2 = nodes[idx_2];
    // If node_2 isn't node_1's neighbor, add node_2
    if (std::find(node_1->neighbors.begin(), node_1->neighbors.end(), node_2) == node_1->neighbors.end()) {
        node_1->neighbors.push_back(node_2);
    }
    // If node_1 isn't node_2's neighbor, add node_1
    if (std::find(node_2->neighbors.begin(), node_2->neighbors.end(), node_1) == node_2->neighbors.end()) {
        node_2->neighbors.push_back(node_1);
    }
}

void Graph::addNodes(const V2D & courses) {
    for (std::vector<std::string> course : courses) {
        nodes.push_back(new GraphNode(course[0]));
    }
}

/**
 * Given a filename to a CSV-formatted text file, create a 2D vector of strings where each row
 * in the text file is a row in the V2D and each comma-separated value is stripped of whitespace
 * and stored as its own string. 
 * 
 * Your V2D should match the exact structure of the input file -- so the first row, first column
 * in the original file should be the first row, first column of the V2D.
 *  
 * @param filename The filename of a CSV-formatted text file. 
 */
V2D file_to_V2D(const std::string & filename){
    // Your code here!
    V2D converted;
    std::vector<std::string> split_by_newline;
    std::string file = file_to_string(filename);
    int rows = SplitString(Trim(file), '\n', split_by_newline);
    for (int row = 0; row < rows; row++) {
        converted.push_back(std::vector<std::string>());
        std::vector<std::string> split_by_comma;
        int cols = SplitString(Trim(split_by_newline[row]), ',', split_by_comma);
        for (int col = 0; col < cols; col++) {
            converted[row].push_back(Trim(split_by_comma[col]));
        }
    }
    return converted;
}

/**
 * Given a course roster and a list of students and their courses, 
 * perform data correction and return a course roster of valid students (and only non-empty courses).
 * 
 * A 'valid student' is a student who is both in the course roster and the student's own listing contains the course
 * A course which has no students (or all students have been removed for not being valid) should be removed
 * 
 * @param cv A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param student A 2D vector of strings where each row is a student ID followed by the courses they are taking
 */
V2D clean(const V2D & cv, const V2D & student){
    // YOUR CODE HERE
    V2D valid_cv;
    std::map<std::string, unsigned> course_to_index;     // Uses one-based indexing to determine non-existent values
    std::map<std::string, unsigned> student_to_index;    // Make sure to subtract 1 from the actual index
    for (unsigned idx = 0; idx < cv.size(); idx++) {
        course_to_index[cv[idx][0]] = idx + 1;
    }
    for (unsigned idx = 0; idx < student.size(); idx++) {
        student_to_index[student[idx][0]] = idx + 1;
    }
    for (unsigned row = 0; row < cv.size(); row++) { // Clean out the invalid entries
        std::string course = cv[row][0];
        valid_cv.push_back(std::vector<std::string>({course}));
        for (unsigned col = 1; col < cv[row].size(); col++) {
            std::string s = cv[row][col];
            if (student_to_index[s] == 0) {
                continue;
            }
            const std::vector<std::string> & student_info = student[student_to_index[s] - 1];
            if (std::find(student_info.begin()++, student_info.end(), course) == student_info.end()) {
                continue;
            }
            valid_cv[row].push_back(s);
        }
    }
    V2D cleaned_cv; // Clean out the empty classes
    for (unsigned row = 0; row < valid_cv.size(); row++) {
        if (valid_cv[row].size() > 1) {
            cleaned_cv.push_back(valid_cv[row]);
        }
    }
    return cleaned_cv;
}

/**
 * Given a collection of courses and a list of available times, create a valid scheduling (if possible).
 * 
 * A 'valid schedule' should assign each course to a timeslot in such a way that there are no conflicts for exams
 * In other words, two courses who share a student should not share an exam time.
 * Your solution should try to minimize the total number of timeslots but should not exceed the timeslots given.
 * 
 * The output V2D should have one row for each timeslot, even if that timeslot is not used.
 * 
 * As the problem is NP-complete, your first scheduling might not result in a valid match. Your solution should 
 * continue to attempt different schedulings until 1) a valid scheduling is found or 2) you have exhausted all possible
 * starting positions. If no match is possible, return a V2D with one row with the string '-1' as the only value. 
 * 
 * @param courses A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param timeslots A vector of strings giving the total number of unique timeslots
 */
V2D schedule(const V2D &courses, const std::vector<std::string> &timeslots){
    // Your code here!
    Graph cv = Graph(courses);
    // Try starting at every node in the course
    for (unsigned start_idx = 0; start_idx < cv.size(); start_idx++) {
        cv.greedyColor(cv.getNode(start_idx));
    }
    // if (cv.maximum_colors <= timeslots.size()) {
    //         break;
    //     } else {
    //         cv.resetColor();
    //     }
    if (cv.maximum_colors > timeslots.size()) {
        V2D empty;
        empty.push_back(std::vector<std::string>({"-1"}));
        return empty;
    }
    V2D sched;
    for (std::string timeslot : timeslots) {
        sched.push_back(std::vector<std::string>({timeslot}));
    }
    // Add each course to the schedule
    for (unsigned idx = 0; idx < cv.size(); idx++) {
        GraphNode* node = cv.getNode(idx);
        sched[node->color].push_back(node->name);
    }
    return sched;
}
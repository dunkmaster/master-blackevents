#include "Graph.h"
#include <fstream>


Graph::Graph(){

}

Graph::Graph(std::vector<Point> points, std::string _xLabel, std::string _yLabel, std::string _name){
	graphPoints = points;
	xLabel = _xLabel;
	yLabel = _yLabel;
	name = _name;
}

Graph::Graph(std::vector< MultiPoint > points, std::vector<std::string> _labels, std::string _name){
    graphMultiPoints = points;
    labels = _labels;
    name = _name;
}

bool Graph::writeMultiGraphToFile(bool append){
    std::ofstream GraphFile;
    if(append){
        GraphFile.open("graph-"+name+".csv", std::ofstream::app);
    } else {
        GraphFile.open("graph-"+name+".csv");
    }

    for (int i = 0; i < labels.size(); ++i)
    {
        GraphFile << labels[i] << ";";
    }
    GraphFile << std::endl;
    for (int i = 0; i < graphMultiPoints.size(); i++)
    {
        for (int j = 0; j < graphMultiPoints[i].size(); ++j)
        {
            GraphFile << graphMultiPoints[i][j] << ";";   
        }
        GraphFile << std::endl;
    }

     GraphFile.close();
}

bool Graph::writeGraphToFile(bool append){
     std::ofstream GraphFile;
    if(append){
        GraphFile.open("graph-"+name+".csv", std::ofstream::app);
    } else {
        GraphFile.open("graph-"+name+".csv");
    }
	         //Opening file to print info to
    GraphFile << xLabel <<";" << yLabel << std::endl;          //Headings for file
    for (int i = 0; i < graphPoints.size(); i++)
    {
    	GraphFile << graphPoints[i].x << ";" << graphPoints[i].y << std::endl;
    }

     GraphFile.close();
}
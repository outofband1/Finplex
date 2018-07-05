========================================================================
    CONSOLE APPLICATION : utitl Project Overview
========================================================================

AppWizard has created this utitl application for you.

This file contains a summary of what you will find in each of the files that
make up your utitl application.


utitl.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

utitl.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

utitl.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named utitl.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////




    /*size_t utilityCount = 0;
    size_t curvePieceCount = 0;
    for (auto& utility : commodities.begin()->second)
    {
        utilityCount++;
        curvePieceCount += utility.first->getCurve().getCurvePieces().size();
    }

    size_t rowCount = 2 + amounts.size() + curvePieceCount; // 2= cost row + spending constraint row

    //                commodities          utilities          constraints - costrow + rhs
    size_t colCount = commodities.size() + utilityCount + rowCount - 1 + 1;

    std::vector<float> tableauData;
    tableauData.resize(colCount * rowCount);

    // setup cost row
    for (size_t i = 0; i < colCount; i++)
    {
        if (i < commodities.size())
        {
            tableauData[i] = 0;
        }
        else if (i < commodities.size() + utilityCount)
        {
            tableauData[i] = -1;
        }
        else
        {
            tableauData[i] = 0;
        }
    }

    size_t colIndex = 0;
    for (auto& commodity : commodities)
    {
        tableauData[(curvePieceCount + 1)*colCount + colIndex] = prices.find(commodity.first)->second;
        tableauData[(curvePieceCount + 1)*colCount + colCount - 1] = money;
        tableauData[(curvePieceCount + 1)*colCount + commodities.size() + utilityCount + curvePieceCount] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityCount = 0;
        size_t pieceCount = 0;
        for (auto& utility : commodity.second)
        {
            float factor = utility.second;
            const Utility::PieceWiseLinearCurve& curve = utility.first->getCurve();

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
                tableauData[rowIndex * colCount + commodities.size() + utilityCount] = 1.0;
                tableauData[rowIndex * colCount + commodities.size() + commodity.second.size() + pieceCount] = 1.0;
                pieceCount++;
                rowIndex++;
            }
            utilityCount++;
        }

        colIndex++;
    }

    size_t amountConstraintCount = 0;
    size_t rowindex = curvePieceCount + 1 + 1; // plus 1 for cost function and plus 1 for money constraint
    colIndex = 0;
    for (auto& commodity : commodities)
    {
        const auto& amount = amounts.find(commodity.first);

        if (amount != amounts.end())
        {
            tableauData[(rowindex)* colCount + colIndex] = 1;
            tableauData[(rowindex)*colCount + commodities.size() + utilityCount + curvePieceCount + colIndex + 1] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = amount->second;
            colIndex++;
            rowindex++;
        }




    }

    SimplexSolver<float> m(rowCount, colCount);

    for (int repeat = 0; repeat < 1; repeat++)
    {
        m.setTableau(tableauData);

        m.printTableau();

        float res;
        std::vector<float> var;

        m.Solve(res, var);

        m.printTableau();

        std::cout << "Result: " << res << std::endl;

        int i = 0;
        for (auto& v : var)
        {
            std::cout << "basic " << i << " = " << v << std::endl;
            i++;
        }
    }*/
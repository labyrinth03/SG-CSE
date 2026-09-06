#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include "database.h"

using namespace std;

MYSQL* conn;

string getEnvOrDefault(const char* name, const string& defaultValue) {
    const char* value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return defaultValue;
    }
    return value;
}

bool connectDB() {
    cout << "Program started." << endl;
    conn = mysql_init(NULL);

    if (conn == NULL) {
        cout << "mysql_init() failed." << endl;
        return false;
    }

    unsigned int timeout = 5;
    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    string host = getEnvOrDefault("MYSQL_HOST", "127.0.0.1");
    string user = getEnvOrDefault("MYSQL_USER", "admin");
    string password = getEnvOrDefault("MYSQL_PASSWORD", getEnvOrDefault("MYSQL_PWD", "1234"));
    string database = getEnvOrDefault("MYSQL_DATABASE", "project2");
    unsigned int port = static_cast<unsigned int>(stoi(getEnvOrDefault("MYSQL_PORT", "3306")));

    if (mysql_real_connect(
            conn,
            host.c_str(),
            user.c_str(),
            password.c_str(),
            database.c_str(),
            port,
            NULL,
            CLIENT_MULTI_STATEMENTS) == NULL) {

        cout << "Connection failed." << endl;
        cout << mysql_error(conn) << endl;
        mysql_close(conn);
        return false;
    }

    cout << "Connected to database." << endl;
    return true;
}

void disconnectDB() {
    if (conn != NULL) {
        mysql_close(conn);
    }
}

void executeQuery(const char* query) {
    if (mysql_query(conn, query)) {
        cout << "Query execution failed." << endl;
        cout << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (result == NULL) {
        if (mysql_field_count(conn) == 0) {
            cout << "Query executed successfully." << endl;
        } else {
            cout << "Result retrieval failed." << endl;
            cout << mysql_error(conn) << endl;
        }
        return;
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    MYSQL_ROW row;
    vector<vector<string> > rows;
    vector<size_t> widths(num_fields);

    for (int i = 0; i < num_fields; i++) {
        widths[i] = string(fields[i].name).length();
    }

    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        vector<string> values;

        for (int i = 0; i < num_fields; i++) {
            if (row[i] == NULL) {
                values.push_back("NULL");
            } else {
                values.push_back(string(row[i], lengths[i]));
            }

            if (values[i].length() > widths[i]) {
                widths[i] = values[i].length();
            }
        }
        rows.push_back(values);
    }

    cout << left;
    for (int i = 0; i < num_fields; i++) {
        cout << setw(static_cast<int>(widths[i])) << fields[i].name;
        if (i < num_fields - 1) {
            cout << " | ";
        }
    }
    cout << endl;

    for (int i = 0; i < num_fields; i++) {
        cout << string(widths[i], '-');
        if (i < num_fields - 1) {
            cout << "-+-";
        }
    }
    cout << endl;

    for (size_t r = 0; r < rows.size(); r++) {
        for (int c = 0; c < num_fields; c++) {
            cout << setw(static_cast<int>(widths[c])) << rows[r][c];
            if (c < num_fields - 1) {
                cout << " | ";
            }
        }
        cout << endl;
    }

    if (rows.empty()) {
        cout << "(no rows)" << endl;
    }

    cout << right;
    mysql_free_result(result);
}

void displayMenu() {
    cout << "\n===== Sample Query Menu =====" << endl;
    cout << "1. Sales Trends" << endl;
    cout << "2. Defective Part Tracking" << endl;
    cout << "3. Top 2 Brands by Revenue" << endl;
    cout << "4. Top 2 Brands by Unit Sales" << endl;
    cout << "5. Seasonal Sales Patterns" << endl;
    cout << "6. Dealer Inventory Efficiency" << endl;
    cout << "7. Supplier Coverage" << endl;
    cout << "0. Exit" << endl;
    cout << "Select: ";
}

int getUserChoice() {
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        return -1;
    }
    return choice;
}

void executeSalesTrendsQuery() {
    cout << "\n===== Year / Gender =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            c.gender,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), c.gender
        ORDER BY sale_year, brand, c.gender;
    )");

    cout << "\n===== Year / Income Range =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            CASE
                WHEN c.annual_income < 50000 THEN 'Low'
                WHEN c.annual_income BETWEEN 50000 AND 80000 THEN 'Middle'
                ELSE 'High'
            END AS income_range,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), income_range
        ORDER BY sale_year, brand, income_range;
    )");

    cout << "\n===== Month / Gender =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            MONTH(st.date) AS sale_month,
            c.gender,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), MONTH(st.date), c.gender
        ORDER BY sale_year, sale_month, brand, c.gender;
    )");

    cout << "\n===== Month / Income Range =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            MONTH(st.date) AS sale_month,
            CASE
                WHEN c.annual_income < 50000 THEN 'Low'
                WHEN c.annual_income BETWEEN 50000 AND 80000 THEN 'Middle'
                ELSE 'High'
            END AS income_range,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), MONTH(st.date), income_range
        ORDER BY sale_year, sale_month, brand, income_range;
    )");

    cout << "\n===== Week / Gender =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            WEEK(st.date) AS sale_week,
            c.gender,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), WEEK(st.date), c.gender
        ORDER BY sale_year, sale_week, brand, c.gender;
    )");

    cout << "\n===== Week / Income Range =====" << endl;

    executeQuery(R"(
        SELECT
            b.name AS brand,
            YEAR(st.date) AS sale_year,
            WEEK(st.date) AS sale_week,
            CASE
                WHEN c.annual_income < 50000 THEN 'Low'
                WHEN c.annual_income BETWEEN 50000 AND 80000 THEN 'Middle'
                ELSE 'High'
            END AS income_range,
            COUNT(*) AS units_sold,
            SUM(st.sale_price) AS total_sales
        FROM Sales_Transactions st
        JOIN Customers c ON st.customer_ID = c.customer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 3 YEAR)
        GROUP BY b.name, YEAR(st.date), WEEK(st.date), income_range
        ORDER BY sale_year, sale_week, brand, income_range;
    )");
}

void executeDefectivePartTrackingQuery() {
    executeQuery(R"(
        SELECT
            v.VIN,
            c.name AS customer_name,
            c.phone,
            c.address,
            s.name AS supplier_name,
            mp.name AS plant_name,
            p.part_name,
            p.part_type,
            p.manufactured_date AS part_manufactured_date,
            st.date AS sale_date
        FROM Suppliers s
        JOIN Parts p ON s.supplier_ID = p.supplier_ID
        JOIN Manufacturing_Plants mp ON p.plant_ID = mp.plant_ID
        JOIN Vehicle_Parts vp ON p.part_ID = vp.part_ID
        JOIN Vehicles v ON vp.VIN = v.VIN
        JOIN Sales_Transactions st ON v.VIN = st.VIN
        JOIN Customers c ON st.customer_ID = c.customer_ID
        WHERE s.name = 'Getrag'
          AND p.part_type = 'Transmission'
          AND p.manufactured_date BETWEEN '2025-01-01' AND '2026-12-31'
        ORDER BY v.VIN;
    )");
}

void executeTopBrandsRevenueQuery() {
    executeQuery(R"(
        SELECT
            b.name AS brand,
            SUM(st.sale_price) AS total_revenue
        FROM Sales_Transactions st
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 1 YEAR)
        GROUP BY b.name
        ORDER BY total_revenue DESC
        LIMIT 2;
    )");
}

void executeTopBrandsUnitSalesQuery() {
    executeQuery(R"(
        SELECT
            b.name AS brand,
            COUNT(*) AS units_sold
        FROM Sales_Transactions st
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        JOIN Brands b ON m.brand_ID = b.brand_ID
        WHERE st.date >= DATE_SUB(CURDATE(), INTERVAL 1 YEAR)
        GROUP BY b.name
        ORDER BY units_sold DESC
        LIMIT 2;
    )");
}

void executeSeasonalSalesPatternQuery() {
    executeQuery(R"(
        SELECT
            MONTH(st.date) AS sale_month,
            COUNT(*) AS units_sold
        FROM Sales_Transactions st
        JOIN Vehicles v ON st.VIN = v.VIN
        JOIN Models m ON v.model_ID = m.model_ID
        WHERE m.body_style = 'Convertible'
        GROUP BY MONTH(st.date)
        ORDER BY units_sold DESC
        LIMIT 1;
    )");
}

void executeDealerInventoryEfficiencyQuery() {
    executeQuery(R"(
        SELECT
            d.name AS dealer,
            AVG(DATEDIFF(st.date, v.manufacturing_date)) AS avg_inventory_days
        FROM Dealers d
        JOIN Sales_Transactions st ON d.dealer_ID = st.dealer_ID
        JOIN Vehicles v ON st.VIN = v.VIN
        GROUP BY d.dealer_ID, d.name
        ORDER BY avg_inventory_days DESC;
    )");
}

void executeSupplierCoverageQuery() {
    executeQuery(R"(
        SELECT
            s.name AS supplier,
            COUNT(DISTINCT sm.model_ID) AS model_count
        FROM Suppliers s
        JOIN Supplier_Model sm ON s.supplier_ID = sm.supplier_ID
        GROUP BY s.supplier_ID, s.name
        ORDER BY model_count DESC
        LIMIT 1;
    )");
}

void exitProgram() {
    disconnectDB();
    cout << "Program ended." << endl;
}

int main() {
    if (!connectDB()) {
        return 1;
    }

    while (true) {
        displayMenu();
        int choice = getUserChoice();

        switch (choice) {
        case 1:
            executeSalesTrendsQuery();
            break;
        case 2:
            executeDefectivePartTrackingQuery();
            break;
        case 3:
            executeTopBrandsRevenueQuery();
            break;
        case 4:
            executeTopBrandsUnitSalesQuery();
            break;
        case 5:
            executeSeasonalSalesPatternQuery();
            break;
        case 6:
            executeDealerInventoryEfficiencyQuery();
            break;
        case 7:
            executeSupplierCoverageQuery();
            break;
        case 0:
            exitProgram();
            return 0;
        default:
            cout << "Invalid number selected." << endl;
        }
    }

    return 0;
}

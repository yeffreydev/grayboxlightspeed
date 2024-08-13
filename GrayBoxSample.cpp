// GrayBoxSample.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "GrayBoxSample.h"
#include "MainDlg.h"
#include <sstream>
#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void StartHttpApp()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
        .name("hello")([]
                       { return "Hello World!"; });

    CROW_ROUTE(app, "/about")
    ([]()
     {
            try {
                L_Account* account = L_GetAccount();
                long correlationId = 0;
                
                L_AddMessageToExtensionWnd(std::to_string(correlationId).c_str());

                // Resto del c�digo de manejo de solicitudes...

                account->L_SendOrderBasic(
                    "LAES",
                    L_OrderType::LIMIT,
                    L_Side::BUY,
                    100,
                    L_PriceBase::abs,
                    0.45,
                    "NSDQ",
                    L_TIF::DAY,
                    false,
                    0,
                    0.0,
                    0,
                    &correlationId
                );
               
                L_AddMessageToExtensionWnd("sending order from request");
                L_AddMessageToExtensionWnd(std::to_string(correlationId).c_str());
                
                return std::to_string(correlationId).c_str();
            }
            catch (const std::exception& e) {
                return "error";
            }});

    CROW_ROUTE(app, "/order").methods("POST"_method)([](const crow::request &req)
                                                     {
        auto x = crow::json::load(req.body);

        if (!x) {
            return crow::response(400, "Invalid JSON format");
        }

        // Extraer los parámetros del JSON
        std::string sideStr = x["side"].s();          // "BUY" o "SELL"
        double entryPrice = x["entryPrice"].d();      // Precio de entrada
        std::string symbol = x["symbol"].s();         // Símbolo de la acción
        unsigned long qty = x["qty"].i();             // Cantidad de acciones

        // Convertir el lado de la orden en el tipo adecuado
        char side = (sideStr == "BUY") ? L_Side::BUY : L_Side::SELL;

        // Obtener la cuenta (simulado)
        L_Account* account = L_GetAccount();

        // Crear un correlationId para capturar el ID de la orden
        long correlationId = 0;

        // Enviar la orden
        account->L_SendOrderBasic(
            symbol.c_str(),
            L_OrderType::LIMIT,
            side,
            qty,
            L_PriceBase::abs,
            entryPrice,
            "NSDQ",
            L_TIF::DAY,
            false,
            0,
            0.0,
            0,
            &correlationId
        );

        // Agregar un mensaje de éxito
        L_AddMessageToExtensionWnd("Order sent from JSON request");

        // Responder al cliente
        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Order sent successfully.";
        response["orderId"] = correlationId;  // Incluir el ID de la orden en la respuesta

        return crow::response{ response }; });

    // simple json response
    CROW_ROUTE(app, "/positions")
    ([]
     {
        L_Account* a = L_GetAccount();
        a->positions_begin();
        for (position_iterator it(a->positions_begin()), ite(a->positions_end()); it != ite; ++it)
        {
            L_AddMessageToExtensionWnd((*it)->L_Symbol(), (*it)->L_TotalPrice());
        }
        crow::json::wvalue x({ {"message", "Hello, World!"} });
        x["message2"] = "Hello, World.. Again!";
        return x; });

    // simple json response
    CROW_ROUTE(app, "/json")
    ([]
     {
        crow::json::wvalue x({ {"message", "Hello, World!"} });
        x["message2"] = "Hello, World.. Again!";
        return x; });

    CROW_ROUTE(app, "/json-initializer-list-constructor")
    ([]
     {
        crow::json::wvalue r({
            {"first", "Hello world!"},                     /* stores a char const* hence a json::type::String */
            {"second", std::string("How are you today?")}, /* stores a std::string hence a json::type::String. */
            {"third", 54},                                 /* stores an int (as 54 is an int literal) hence a std::int64_t. */
            {"fourth", (int64_t)54l},                               /* stores a long (as 54l is a long literal) hence a std::int64_t. */
            {"fifth", 54u},                                /* stores an unsigned int (as 54u is a unsigned int literal) hence a std::uint64_t. */
            {"sixth", (uint64_t)54ul},                               /* stores an unsigned long (as 54ul is an unsigned long literal) hence a std::uint64_t. */
            {"seventh", 2.f},                              /* stores a float (as 2.f is a float literal) hence a double. */
            {"eighth", 2.},                                /* stores a double (as 2. is a double literal) hence a double. */
            {"ninth", nullptr},                            /* stores a std::nullptr hence json::type::Null . */
            {"tenth", true}                                /* stores a bool hence json::type::True . */
            });
        return r; });

    // json list response
    CROW_ROUTE(app, "/json_list")
    ([]
     {
        crow::json::wvalue x(crow::json::wvalue::list({ 1, 2, 3 }));
        return x; });

    CROW_ROUTE(app, "/hello/<int>")
    ([](int count)
     {
        if (count > 100)
            return crow::response(400);
        std::ostringstream os;
        os << count << " bottles of beer!";
        return crow::response(os.str()); });

    // example which uses only response as a paramter without
    // request being a parameter.
    CROW_ROUTE(app, "/add/<int>/<int>")
    ([](crow::response &res, int a, int b)
     {
        std::ostringstream os;
        os << a + b;
        res.write(os.str());
        res.end(); });

    // Compile error with message "Handler type is mismatched with URL paramters"
    // CROW_ROUTE(app,"/another/<int>")
    //([](int a, int b){
    // return crow::response(500);
    //});

    // more json example
    CROW_ROUTE(app, "/add_json")
        .methods("POST"_method)([](const crow::request &req)
                                {
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        int sum = x["a"].i() + x["b"].i();
        std::ostringstream os;
        os << sum;
        return crow::response{ os.str() }; });

    CROW_ROUTE(app, "/params")
    ([](const crow::request &req)
     {
        std::ostringstream os;
        os << "Params: " << req.url_params << "\n\n";
        os << "The key 'foo' was " << (req.url_params.get("foo") == nullptr ? "not " : "") << "found.\n";
        if (req.url_params.get("pew") != nullptr)
        {
            double countD = crow::utility::lexical_cast<double>(req.url_params.get("pew"));
            os << "The value of 'pew' is " << countD << '\n';
        }
        auto count = req.url_params.get_list("count");
        os << "The key 'count' contains " << count.size() << " value(s).\n";
        for (const auto& countVal : count)
        {
            os << " - " << countVal << '\n';
        }
        return crow::response{ os.str() }; });

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::Debug);
    // crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .server_name("CrowCpp")
        .multithreaded()
        .run();
}

// CGrayBoxSampleApp

void CGrayBoxSampleApp::StartExtension()
{
    L_AddMessageToExtensionWnd("START SERVER PORT 18080");
    std::thread(StartHttpApp).detach();
}
void CGrayBoxSampleApp::StopExtension()
{
    if (m_pMainWnd)
    {
        m_pMainWnd->DestroyWindow();
        m_pMainWnd = 0;
    }
}

BEGIN_MESSAGE_MAP(CGrayBoxSampleApp, CWinApp)
END_MESSAGE_MAP()

// CGrayBoxSampleApp construction

CGrayBoxSampleApp::CGrayBoxSampleApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CGrayBoxSampleApp object

CGrayBoxSampleApp theApp;

// CGrayBoxSampleApp initialization

LSEXPORT void LSInitInstance()
{
    theApp.StartExtension();
}
LSEXPORT void LSExitInstance()
{
    theApp.StopExtension();
}
LSEXPORT BOOL LSPreTranslateMessage(MSG *pMsg)
{
    return theApp.PreTranslateMessage(pMsg);
}

BOOL CGrayBoxSampleApp::InitInstance()
{
    CWinApp::InitInstance();

    return TRUE;
}

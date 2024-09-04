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



void CGrayBoxSampleApp::StartHttpApp()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")
		.name("hello")([]
			{ return "Hello World!"; });

	CROW_ROUTE(app, "/about")
		([this]()
			{
				try {
				
					long correlationId = 0;

					L_AddMessageToExtensionWnd(std::to_string(correlationId).c_str());

					// Resto del c�digo de manejo de solicitudes...

					this->account->L_SendOrderBasic(
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
				} });

	CROW_ROUTE(app, "/order").methods("POST"_method)([](const crow::request& req)
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
			std::string market = x["market"].s();
			std::string market2 = x["market2"].s();
			std::string priceBaseStr = x["base"].s();
			// Convertir el lado de la orden en el tipo adecuado
			char side = (sideStr == "BUY") ? L_Side::BUY : L_Side::SELL;

			
			
			L_PriceBase::L_PriceBaseType pricebase = (priceBaseStr == "bid") ? L_PriceBase::bid : (priceBaseStr == "ask") ?
				L_PriceBase::ask : (priceBaseStr == "mid") ? L_PriceBase::mid : (priceBaseStr == "last") ? L_PriceBase::last : (priceBaseStr == "open") ? L_PriceBase::open : (priceBaseStr == "close") ?
				L_PriceBase::close : (priceBaseStr == "rbid") ? L_PriceBase::rbid : (priceBaseStr == "rask") ? L_PriceBase::rask : (priceBaseStr == "rmid") ? L_PriceBase::rmid : (priceBaseStr == "rlast") ?
				L_PriceBase::rlast : (priceBaseStr == "ropen") ? L_PriceBase::ropen : (priceBaseStr == "rclose") ? L_PriceBase::rclose : L_PriceBase::abs;
			// Obtener la cuenta (simulado)
			L_Account* account = L_GetAccount();

			// Crear un correlationId para capturar el ID de la orden
			long correlationId = 0;
			L_AddMessageToExtensionWnd(market.c_str());

			// Enviar la orden
			account->L_SendOrderBasic(
				symbol.c_str(),
				L_OrderType::LIMIT,
				side,
				qty,
				pricebase,
				entryPrice,
				market.c_str(),
				L_TIF::DAY,
				false,
				0,
				0.0,
				market2.c_str(),
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
				// Obtén la cuenta
				L_Account* account = L_GetAccount();

				// Obtén el iterador de posiciones
				position_iterator itStart = account->open_positions_begin();
				position_iterator itEnd = account->open_positions_end();

				// Crear un JSON array
				crow::json::wvalue positions_json;

				std::vector<crow::json::wvalue> vec = {};

				// Iterar sobre las posiciones
				for (position_iterator it(itStart); it != itEnd; ++it)
				{
					// Crear un JSON object para la posición

					crow::json::wvalue position_json;
					position_json["symbol"] = (*it)->L_Symbol(),//symbol
						position_json["shares"] = (*it)->L_Shares(),//quantity
						position_json["average_price"] = (*it)->L_AveragePrice(),//entry price
						position_json["total_price"] = (*it)->L_TotalPrice(),
						position_json["last_price"] = (*it)->L_PLLastPrice(),
						position_json["cost_basis_traditional"] = (*it)->L_CostBasisTraditional(),
					position_json["money_invested"] = (*it)->L_MoneyInvested(),
					position_json["value"] = (*it)->L_Value(),
					position_json["marked_pl"] = (*it)->L_MarkedPL();
					position_json["open_pl"] = (*it)->L_OpenPL();
					// Agregar el JSON object a la lista de posiciones
					vec.push_back(position_json);

				}
				// Retorna el JSON con todas las posiciones
				crow::json::wvalue x(vec);
				return x;});

	CROW_ROUTE(app, "/orders")
		([]
			{
				// Obtén la cuenta
				L_Account* account = L_GetAccount();

				// Obtén el iterador de posiciones
				order_iterator itStart = account->orders_begin();
				order_iterator itEnd = account->orders_end();

				// Crear un JSON array
				crow::json::wvalue positions_json;

				std::vector<crow::json::wvalue> vec = {};

				// Iterar sobre las posiciones
				for (order_iterator it(itStart); it != itEnd; ++it)
				{
					// Crear un JSON object para la posición

					crow::json::wvalue order_json;
					order_json["average_price"] = (*it)->L_AveragePrice();
					order_json["executed_shares"] = (*it)->L_ExecutedShares();
					order_json["active_shares"] = (*it)->L_ActiveShares();
					order_json["orderId"] = (*it)->L_ReferenceId();
					order_json["symbol"] = (*it)->L_Symbol();
					// Agregar el JSON object a la lista de posiciones
					vec.push_back(order_json);

				}
				crow::json::wvalue x(vec);
				return x; });

	CROW_ROUTE(app, "/position/<string>")
		([](const std::string& symbol)
			{
				// Obtén la cuenta
				L_Account* account = L_GetAccount();
				if (!account) {
					return crow::response(500); // Error interno si no se puede obtener la cuenta
				}

				// Encuentra la posición
				L_Position* position = account->L_FindPosition(symbol.c_str());
				if (!position) {
					return crow::response(404); // No se encontró la posición
				}

				// Construye la respuesta JSON
				crow::json::wvalue position_json;
				position_json["symbol"] = position->L_Symbol(),
					position_json["shares"] = position->L_Shares(),
					position_json["average_price"] = position->L_AveragePrice(),
					position_json["total_price"] = position->L_TotalPrice(),
					position_json["last_price"] = position->L_PLLastPrice(),
					position_json["cost_basis_traditional"] = position->L_CostBasisTraditional(),
					position_json["money_invested"] = position->L_MoneyInvested(),
					position_json["value"] = position->L_Value(),
					position_json["marked_pl"] = position->L_MarkedPL();
					position_json["open_pl"] = position->L_OpenPL();


				return crow::response(position_json); });

	CROW_ROUTE(app, "/order/<string>")
		([this](const std::string& correlationIdString)
			{
				// Convierte el string a long
				long correlationId = 0;
				try {
					correlationId = std::stol(correlationIdString);
				}
				catch (const std::invalid_argument& e) {
					return crow::response(400); // Bad request si la conversión falla
				}
				catch (const std::out_of_range& e) {
					return crow::response(400); // Bad request si el valor está fuera del rango
				}

				if (idsMap.count(correlationId) ==0) {
					return crow::response(404);
				}
				IdPair idPair = idsMap[correlationId];
				// Encuentra la orden
				L_Order* order = this->account->L_FindOrder(idPair.id1);
				if (!order) {
					return crow::response(404); // No se encontró la orden
				}

				// Construye la respuesta JSON
				crow::json::wvalue order_json;
				order_json["average_price"] = order->L_AveragePrice();
				order_json["executed_shares"] = order->L_ExecutedShares();
				order_json["active_shares"] = order->L_ActiveShares();
				order_json["orderId"] = order->L_ReferenceId();
				order_json["symbol"] = order->L_Symbol();

				return crow::response(order_json); });

	CROW_ROUTE(app, "/order/<string>").methods("DELETE"_method)([this](const std::string& correlationIdString)
		{
			// Convierte el string a long
			long correlationId = 0;
			try
			{
				correlationId = std::stol(correlationIdString);
			}
			catch (const std::invalid_argument& e)
			{
				return crow::response(400); // Bad request si la conversión falla
			}
			catch (const std::out_of_range& e)
			{
				return crow::response(400); // Bad request si el valor está fuera del rango
			}


			if (idsMap.count(correlationId) == 0) {
				return crow::response(404);
			}
			IdPair idPair = idsMap[correlationId];
			

			L_Order* order = this->account->L_FindOrder(idPair.id1);
			if (!order) {
				return crow::response(404); // No se encontró la orden
			}

			// Cancela la orden
			this->account->L_CancelOrder(order);

			crow::json::wvalue response;
			response["message"] = "Order cancelled successfully.";
			return crow::response(response); // No Content, la orden fue cancelada exitosamente
		});

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
		([](crow::response& res, int a, int b)
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
		.methods("POST"_method)([](const crow::request& req)
			{
				auto x = crow::json::load(req.body);
				if (!x)
					return crow::response(400);
				int sum = x["a"].i() + x["b"].i();
				std::ostringstream os;
				os << sum;
				return crow::response{ os.str() }; });

	CROW_ROUTE(app, "/params")
		([](const crow::request& req)
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
	account = L_GetAccount();
	account->L_Attach(this);
	std::thread(&CGrayBoxSampleApp::StartHttpApp, this).detach();
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

void CGrayBoxSampleApp::HandleMessage(L_Message const* pMsg)
{
	switch (pMsg->L_Type())
	{
	case L_MsgOrderRequested::id:
	{
		L_AddMessageToExtensionWnd("new message order request handleMessage");
		L_MsgOrderRequested const* pMsgOrderRequested = static_cast<L_MsgOrderRequested const*>(pMsg);
		idsMap[pMsgOrderRequested->L_CorrelationId()] = IdPair(pMsgOrderRequested->L_Order1ReferenceId(), pMsgOrderRequested->L_Order2ReferenceId());
	}
	break;
	}
}

// CGrayBoxSampleApp initialization

LSEXPORT void LSInitInstance()
{
	theApp.StartExtension();
}
LSEXPORT void LSExitInstance()
{
	theApp.StopExtension();
}
LSEXPORT BOOL LSPreTranslateMessage(MSG* pMsg)
{
	return theApp.PreTranslateMessage(pMsg);
}

BOOL CGrayBoxSampleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

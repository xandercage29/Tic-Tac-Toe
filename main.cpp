// main.cpp
#include <crow.h>
#include <string>
#include <vector>
#include <memory>

class TicTacToe {
private:
    std::vector<char> board;
    char currentPlayer;
    bool gameOver;
    
    bool checkWin() const {
        // Check rows
        for (int i = 0; i < 9; i += 3) {
            if (board[i] != ' ' && board[i] == board[i + 1] && board[i + 1] == board[i + 2]) {
                return true;
            }
        }
        
        // Check columns
        for (int i = 0; i < 3; i++) {
            if (board[i] != ' ' && board[i] == board[i + 3] && board[i + 3] == board[i + 6]) {
                return true;
            }
        }
        
        // Check diagonals
        if (board[0] != ' ' && board[0] == board[4] && board[4] == board[8]) {
            return true;
        }
        if (board[2] != ' ' && board[2] == board[4] && board[4] == board[6]) {
            return true;
        }
        
        return false;
    }
    
    bool checkDraw() const {
        for (char cell : board) {
            if (cell == ' ') {
                return false;
            }
        }
        return true;
    }
    
public:
    TicTacToe() : board(9, ' '), currentPlayer('X'), gameOver(false) {}
    
    void reset() {
        board = std::vector<char>(9, ' ');
        currentPlayer = 'X';
        gameOver = false;
    }
    
    crow::json::wvalue makeMove(int position) {
        if (position < 0 || position >= 9 || board[position] != ' ' || gameOver) {
            return {
                {"valid", false}
            };
        }
        
        board[position] = currentPlayer;
        bool won = checkWin();
        bool draw = !won && checkDraw();
        
        crow::json::wvalue result = {
            {"valid", true},
            {"player", std::string(1, currentPlayer)},
            {"gameOver", won},
            {"draw", draw},
            {"position", position}
        };
        
        if (!won && !draw) {
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            result["nextPlayer"] = std::string(1, currentPlayer);
        }
        
        return result;
    }
};

int main() {
    crow::SimpleApp app;
    std::shared_ptr<TicTacToe> game = std::make_shared<TicTacToe>();
    
    CROW_ROUTE(app, "/move")
        .methods("POST"_method)
        ([&game](const crow::request& req) {
            auto x = crow::json::load(req.body);
            if (!x) {
                return crow::response(400);
            }
            
            int position = x["position"].i();
            return crow::response(game->makeMove());
    });
    
    CROW_ROUTE(app, "/reset")
        .methods("POST"_method)
        ([&game](const crow::request&) {
            game->reset();
            return crow::response(crow::json::wvalue{{"success", true}});
    });
    
    app.port(8080)
        .multithreaded()
        .run();
        
    return 0;
}

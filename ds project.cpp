#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <sstream> 
#include <set>
#include <string>
#include <algorithm>
#include <map>
#include <stack>
#include <windows.h>
#include <conio.h>

using namespace std;

class User {
private:
    string username;
    string password;

public:
    User(const string& name, const string& pass) : username(name), password(pass) {}

    string getUsername() const {
        return username;
    }

    void SaveUserToFile() {
        ofstream userFile((username + "_user.txt").c_str());
        userFile << username << " " << password << endl;
        userFile.close();
    }
};

class Song {
public:
    string SongName;
    string SongID;
    string Artist;
    string genre;
    User user;

public:
    Song(const string& name, const string& id, const string& artist, const string& genre, const User& u)
        : SongName(name), SongID(id), Artist(artist), genre(genre), user(u) {}

    const User& getUser() const {
        return user;
    }
};

class Playlist {
public:
    string PlaylistName;
    queue<Song> songs;
    stack<Playlist> playlistStack;

public:
    User P_user;

    Playlist(const string& playlistName, const User& user)
        : PlaylistName(playlistName), P_user(user) {}

    void AddSong(Song s) {
        songs.push(s);
    }

    void RemoveSong(string name) {
        bool songFound = false;
        queue<Song> temp;

        while (!songs.empty()) {
            if (songs.front().SongName == name) {
                songFound = true;
            } else {
                temp.push(songs.front());
            }
            songs.pop();
        }

        if (!songFound) {
            cout << "Song '" << name << "' not in the playlist";
        }

        while (!temp.empty()) {
            songs.push(temp.front());
            temp.pop();
        }
    }

    queue<Song>& getSongs() {
        return songs;
    }

    void pushPlaylistIntoStack(stack<Playlist>& stack, const Playlist& existing) {
        Playlist newPlaylist = existing;
        newPlaylist.songs = existing.songs;
        stack.push(newPlaylist);
    }

    void SavePlaylistToFile() {
        ofstream playlistFile((P_user.getUsername() + "_" + PlaylistName + "_playlist.txt").c_str());
        playlistFile << PlaylistName << endl;

        queue<Song> playlistSongs = getSongs();

        while (!playlistSongs.empty()) {
            Song currentSong = playlistSongs.front();
            playlistFile << currentSong.getUser().getUsername() << " "
                         << currentSong.SongName << " "
                         << currentSong.SongID << " "
                         << currentSong.Artist << " "
                         << currentSong.genre << endl;
            playlistSongs.pop();
        }

        playlistFile.close();
    }
};
void SaveData(stack<Playlist> playlists, User currentUser) {
    currentUser.SaveUserToFile();

    stack<Playlist> tempStack = playlists;

    while (!tempStack.empty()) {
        tempStack.top().SavePlaylistToFile();
        tempStack.pop();
    }
}
void CreatePlaylist(stack<Playlist>& playlists, const User& currentUser) {
  string playlistName;
    cout << "Enter the name for the new playlist: ";
    cin >> playlistName;

    Playlist newPlaylist(playlistName, currentUser);

    // Ask how many songs the user wants to add
    int numSongs;
    cout << "How many songs do you want to add to the playlist? ";
    cin >> numSongs;

    for (int i = 0; i < numSongs; ++i) {
        string songName, songID, artist, genre;
        cout << "Enter details for song " << i + 1 << " - Name, ID, Artist, Genre: ";
        cin >> songName >> songID >> artist >> genre;

        Song newSong(songName, songID, artist, genre, currentUser);
        newPlaylist.AddSong(newSong);
    }

    playlists.push(newPlaylist);
    cout << "Playlist '" << playlistName << "' created with " << numSongs << " songs." << endl;
    SaveData(playlists, currentUser);
}

void LoadData(stack<Playlist>& playlists, const User& currentUser) {
    ifstream userFile((currentUser.getUsername() + "_user.txt").c_str());

    if (userFile) {
        userFile.close();  // Close the user file as it's only used for authentication

        // Clear the existing playlist stack
        while (!playlists.empty()) {
            playlists.pop();
        }

        // Open each playlist file and load its content into the stack
        // Playlist file names are constructed as username_playlistName_playlist.txt
        string playlistFileNamePattern = currentUser.getUsername() + "_";
        string line;

        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((playlistFileNamePattern + "*_playlist.txt").c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                string playlistFileName = findFileData.cFileName;
                ifstream playlistFile(playlistFileName.c_str());

                if (playlistFile) {
                    getline(playlistFile, line);  // Read the playlist name
                    Playlist newPlaylist(line, currentUser);

                    while (getline(playlistFile, line)) {
                        istringstream iss(line);
                        string username, songName, songID, artist, genre;

                        iss >> username >> songName >> songID >> artist >> genre;

                        User songUser(username, "");  // Assuming an empty password
                        Song newSong(songName, songID, artist, genre, songUser);

                        newPlaylist.AddSong(newSong);
                    }

                    playlists.push(newPlaylist);
                    playlistFile.close();
                }
            } while (FindNextFile(hFind, &findFileData) != 0);

            FindClose(hFind);
        } else {
            cout << "No playlists found for user '" << currentUser.getUsername() << "'. Creating a new playlist." << endl;
            CreatePlaylist(playlists, currentUser);
        }
    } else {
        cout << "Error reading user file for '" << currentUser.getUsername() << "'. Exiting." << endl;
    }
}







Playlist ChoosePlaylist(User& user, stack<Playlist>& P_stack) {
    stack<Playlist> temp = P_stack;
    int Index = 1;

    while (!temp.empty()) {
        cout << Index << "--->" << temp.top().PlaylistName << endl;
        temp.pop();
        Index++;
    }

    int choice;
    cout << "Enter the number corresponding to the playlist you want to choose: ";
    cin >> choice;

    if (choice >= 1 && choice <= P_stack.size()) {
        temp = P_stack;

        for (int i = 1; i < choice; ++i) {
            temp.pop();
        }

        Playlist Chose_play = temp.top();
        return Chose_play;
    } else {
        cout << "Invalid choice." << endl;
        return P_stack.top();
    }
}

void ChangePlaylistName(stack<Playlist>& playlists, User user) {
    string newName;
    cout << "Enter the new playlist name: ";
    cin >> newName;

    Playlist chosenPlaylist = ChoosePlaylist(user, playlists);

    if (!chosenPlaylist.PlaylistName.empty()) {
        chosenPlaylist.PlaylistName = newName;

        SaveData(playlists, user);

        cout << "Playlist name changed to '" << newName << "'." << endl;
    } else {
        char ch;
        cout << "No playlist found under your username." << endl;
        cout << "Do you want to make a Playlist? (Y/N): ";
        cin >> ch;

        switch (ch) {
            case 'Y':
            case 'y': {
                CreatePlaylist(playlists, user);
                break;
            }
            case 'N':
            case 'n':
                cout << "No playlist created." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }
}

void PlaySong(const Song& song) {
    cout << "Playing--->" << song.SongName << " by " << song.Artist << endl;
    Sleep(100);
    cout << "Song finished." << endl;
}

void listensongs(stack<Playlist>& playlists, User currentUser) {
	system("cls");
	cout<<"You are now listening to music\n";
    Playlist ToBePlayed = ChoosePlaylist(currentUser, playlists);
    queue<Song>& songQueue = ToBePlayed.getSongs();

    while (!songQueue.empty()) {
        PlaySong(songQueue.front());
        songQueue.pop();

        if (songQueue.empty()) {
            cout << "Playlist finished" << endl;
            break;
        }

        cout << "Press 'Space' to play the next song, 'Backspace' to stop and go back to the user menu:\n";
        int key = _getch();

        if (key == 32) {  // ASCII code for space
            PlaySong(songQueue.front());
            songQueue.pop();
        } else if (key == 8) {  // ASCII code for backspace
            cout << "Do you want to continue listening ? Y/N: ";
            char choice;
            cin >> choice;

            if (choice == 'N' || choice == 'n') {
                cout << "Stopping playback.....Returning to the user menu." << endl;
                break;
            }
        } else {
            cout << "Invalid key pressed. Stopping playback. Returning to the user menu" << endl;
            break;
        }
    }
}
void SearchByGenre(stack<Playlist>& playlists, const User& currentUser) {
    string genre;
    cout << "Enter the genre to search for: ";
    cin >> genre;

    queue<Song> matchingSongs;

    // Iterate through playlists and find songs with the specified genre
    stack<Playlist> tempStack = playlists;
    bool genreFound = false;

    while (!tempStack.empty()) {
        queue<Song> playlistSongs = tempStack.top().getSongs();
        while (!playlistSongs.empty()) {
            if (playlistSongs.front().genre == genre) {
                matchingSongs.push(playlistSongs.front());
                genreFound = true;
            }
            playlistSongs.pop();
        }
        tempStack.pop();
    }

    // Display matching songs if genre is found
    if (genreFound) {
        cout << "Matching songs with genre '" << genre << "':" << endl;
        while (!matchingSongs.empty()) {
            Song currentSong = matchingSongs.front();
            cout << "\n\tSong Name: " << currentSong.SongName << ", Artist: " << currentSong.Artist << endl;
            matchingSongs.pop();
        }
    } else {
        char choice;
        cout << "Genre not found. Do you want to keep searching? (Y/N): ";
        cin >> choice;

        if (choice == 'Y' || choice == 'y') {
            // Recursively call the function to continue searching
            SearchByGenre(playlists, currentUser);
        } else {
            cout << "Search stopped. Returning to the user menu." << endl;
            // You might want to add appropriate handling or return to the user menu here
        }
    }
}
// Function to calculate genre similarity
double GenreSimilarity( Playlist& playlist) {
    int maxGenreCount = 0;
    string mostFrequentGenre;

    // Count occurrences of each genre in the playlist
    queue<Song> playlistSongs = playlist.getSongs();
    map<string, int> genreCount;

    while (!playlistSongs.empty()) {
        string currentGenre = playlistSongs.front().genre;
        genreCount[currentGenre]++;
        
        if (genreCount[currentGenre] > maxGenreCount) {
            maxGenreCount = genreCount[currentGenre];
            mostFrequentGenre = currentGenre;
        }

        playlistSongs.pop();
    }

    return (double)maxGenreCount / playlist.getSongs().size();
}

// Function to calculate artist similarity
double ArtistSimilarity(Playlist& playlist) {
    int maxArtistCount = 0;
    string mostFrequentArtist;

    // Count occurrences of each artist in the playlist
    queue<Song> playlistSongs = playlist.getSongs();
    map<string, int> artistCount;

    while (!playlistSongs.empty()) {
        string currentArtist = playlistSongs.front().Artist;
        artistCount[currentArtist]++;
        
        if (artistCount[currentArtist] > maxArtistCount) {
            maxArtistCount = artistCount[currentArtist];
            mostFrequentArtist = currentArtist;
        }

        playlistSongs.pop();
    }

    return (double)maxArtistCount / playlist.getSongs().size();
}

// Function to generate personalized recommendations
void PersonalizedRecommendations(stack<Playlist>& playlists, const User& currentUser) {
    stack<Playlist> tempStack = playlists;

    // Check if the user has more than one playlist
    if (tempStack.size() > 1) {
        double genreSimilarity = GenreSimilarity(tempStack.top());
        double artistSimilarity = ArtistSimilarity(tempStack.top());

        cout << "Genre Similarity: " << genreSimilarity << endl;
        cout << "Artist Similarity: " << artistSimilarity << endl;

        // Check if genre or artist similarity is high
        if (genreSimilarity > 0.5 || artistSimilarity > 0.5) {
            cout << "AI recommends creating a new playlist based on genre or artist similarity." << endl;

            // Create temporary playlists for genre and artist similarity
            Playlist genreSimilarPlaylist("GenreSimilarPlaylist", currentUser);
            Playlist artistSimilarPlaylist("ArtistSimilarPlaylist", currentUser);

            // Iterate through the user's playlists and add songs to temporary playlists
            while (!tempStack.empty()) {
                if (GenreSimilarity(tempStack.top()) > 0.5) {
                    genreSimilarPlaylist.AddSong(tempStack.top().getSongs().front());
                }

                if (ArtistSimilarity(tempStack.top()) > 0.5) {
                    artistSimilarPlaylist.AddSong(tempStack.top().getSongs().front());
                }

                tempStack.pop();
            }

            // Display the temporary playlists
            cout << "Songs based on Genre Similarity:" << endl;
            queue<Song> genreSongs = genreSimilarPlaylist.getSongs();
            while (!genreSongs.empty()) {
                Song currentSong = genreSongs.front();
                cout << "Song Name: " << currentSong.SongName << ", Artist: " << currentSong.Artist << endl;
                genreSongs.pop();
            }

            cout << "Songs based on Artist Similarity:" << endl;
            queue<Song> artistSongs = artistSimilarPlaylist.getSongs();
            while (!artistSongs.empty()) {
                Song currentSong = artistSongs.front();
                cout << "Song Name: " << currentSong.SongName << ", Artist: " << currentSong.Artist << endl;
                artistSongs.pop();
            }

            // Ask the user if they want to add the playlists
            cout << "Do you want to add these playlists? (Y/N): ";
            char choice;
            cin >> choice;

            if (choice == 'Y' || choice == 'y') {
                // Ask for playlist names
                string genrePlaylistName, artistPlaylistName;
                cout << "Enter a name for the playlist based on Genre Similarity: ";
                cin >> genrePlaylistName;
                cout << "Enter a name for the playlist based on Artist Similarity: ";
                cin >> artistPlaylistName;

                // Add playlists to the stack
                genreSimilarPlaylist.PlaylistName = genrePlaylistName;
                artistSimilarPlaylist.PlaylistName = artistPlaylistName;

                playlists.push(genreSimilarPlaylist);
                playlists.push(artistSimilarPlaylist);

                cout << "Playlists added successfully!" << endl;
            } else {
                cout << "Playlists not added. Returning to the user menu." << endl;
            }
        } else {
            cout << "Genre and artist similarity are not significant. No recommendations available." << endl;
        }
    } else {
        cout << "Not enough playlists to generate personalized recommendations." << endl;
    }
}


// Other functions...

// Function to search songs by artist
void SearchByArtist(stack<Playlist>& playlists, const User& currentUser) {
    string artist;
    cout << "Enter the artist to search for: ";
    cin >> artist;

    queue<Song> matchingSongs;

    // Iterate through playlists and find songs with the specified artist
    stack<Playlist> tempStack = playlists;
    bool artistFound = false;

    while (!tempStack.empty()) {
        queue<Song> playlistSongs = tempStack.top().getSongs();
        while (!playlistSongs.empty()) {
            if (playlistSongs.front().Artist == artist) {
                matchingSongs.push(playlistSongs.front());
                artistFound = true;
            }
            playlistSongs.pop();
        }
        tempStack.pop();
    }

    // Display matching songs if artist is found
    if (artistFound) {
        cout << "Matching songs by artist '" << artist << "':" << endl;
        while (!matchingSongs.empty()) {
            Song currentSong = matchingSongs.front();
            cout << "Song Name: " << currentSong.SongName << ", Artist: " << currentSong.Artist << endl;
            matchingSongs.pop();
        }
    } else {
        char choice;
        cout << "Artist not found. Do you want to keep searching? (Y/N): ";
        cin >> choice;

        if (choice == 'Y' || choice == 'y') {
            // Recursively call the function to continue searching
            SearchByArtist(playlists, currentUser);
        } else {
            cout << "Search stopped. Returning to the user menu." << endl;
            // You might want to add appropriate handling or return to the user menu here
        }
    }
}
void SearchAllPlaylists(stack<Playlist>& playlists, const User& currentUser) {
    queue<Song> allSongs;

    // Iterate through playlists and gather all songs
    stack<Playlist> tempStack = playlists;
    while (!tempStack.empty()) {
        queue<Song> playlistSongs = tempStack.top().getSongs();
        while (!playlistSongs.empty()) {
            allSongs.push(playlistSongs.front());
            playlistSongs.pop();
        }
        tempStack.pop();
    }

    // Display all songs
    cout << "All songs across all playlists:" << endl;
    while (!allSongs.empty()) {
        Song currentSong = allSongs.front();
        cout << "Song Name: " << currentSong.SongName << ", Artist: " << currentSong.Artist << endl;
        allSongs.pop();
    }
}

void SearchSongs(stack<Playlist>& playlists, const User& currentUser) {
	system("cls");
	cout<<"Searching Songs..";
    int searchChoice;
    cout << "Choose search option:\n";
    cout << "1. Search by Genre\n";
    cout << "2. Search by Artist\n";
    cout << "3. Search across all playlists\n";
    cout << "Enter your choice (1, 2, or 3): ";
    cin >> searchChoice;

    switch (searchChoice) {
        case 1:
            SearchByGenre(playlists, currentUser);
            break;
        case 2:
            SearchByArtist(playlists, currentUser);
            break;
        case 3:
            SearchAllPlaylists(playlists, currentUser);
            break;
        default:
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
            break;
    }
}


void AddSongsToPlaylist(Playlist& playlist) {
    char addSongChoice;
    do {
        string songName, songID, artist, genre;
        cout << "Enter song details - Name, ID, Artist, Genre: ";
        cin >> songName >> songID >> artist >> genre;

        Song newSong(songName, songID, artist, genre, playlist.P_user);
        playlist.AddSong(newSong);

        cout << "Do you want to add another song? (Y/N): ";
        cin >> addSongChoice;
    } while (addSongChoice == 'Y' || addSongChoice == 'y');
}
void EditPlaylist(stack<Playlist>& playlists, User user) {
	system("cls");
    Playlist chosenPlaylist = ChoosePlaylist(user, playlists);

    if (!chosenPlaylist.PlaylistName.empty()) {
        char editChoice;
        do {
            // Display playlist contents
            cout << "Playlist '" << chosenPlaylist.PlaylistName << "' contents:\n";
            queue<Song> playlistSongs = chosenPlaylist.getSongs();
            int songIndex = 1;
            cout<<"Your Playlists are displayed\n";
            while (!playlistSongs.empty()) {
                Song currentSong = playlistSongs.front();
                cout << songIndex << ". Song Name: " << currentSong.SongName
                     << ", Artist: " << currentSong.Artist << endl;
                playlistSongs.pop();
                songIndex++;
            }

            // Ask user for action
            cout << "\nChoose an action:\n";
            cout << "1. Add a song\n";
            cout << "2. Delete a song\n";
            cout << "3. Change playlist name\n";
            cout << "4. Delete entire playlist\n";
            cout << "5. Exit editing\n";
            cout << "Enter your choice (1-5): ";
            cin >> editChoice;

            switch (editChoice) {
                case '1':
                    AddSongsToPlaylist(chosenPlaylist);
                    SaveData(playlists, user);
                    break;
                case '2': {
                    string songNameToDelete;
                    cout << "Enter the name of the song to delete: ";
                    cin >> songNameToDelete;
                    chosenPlaylist.RemoveSong(songNameToDelete);
                    SaveData(playlists, user);
                    break;
                }
                case '3': {
                    string newName;
                    cout << "Enter the new playlist name: ";
                    cin >> newName;
                    chosenPlaylist.PlaylistName = newName;
                    SaveData(playlists, user);
                    cout << "Playlist name changed to '" << newName << "'.\n";
                    break;
                }
                case '4':
                    playlists.pop();  // Remove the chosen playlist
                    SaveData(playlists, user);
                    cout << "Playlist '" << chosenPlaylist.PlaylistName << "' deleted.\n";
                    break;
                case '5':
                    cout << "Exiting editing.\n";
                    break;
                default:
                    cout << "Invalid choice. Please enter a number from 1 to 5.\n";
            }
        } while (editChoice != '5');
    } else {
        cout << "No playlist found under your username.\n";
    }
}
void userMenu(stack<Playlist>& playlists, const User& currentUser) {
    char choice;

    cout << "Choose an option:\n";
    cout << "1. Make a Playlist\n";
    cout << "2. Edit a Playlist\n";
    cout << "3. Listen to Songs\n";
    cout << "4. Search Songs\n";
	cout<<  "5. Personalized Recommendations\n"; // Added option for searching songs
    cout << "Enter your choice (1, 2, 3,4 or 5): ";
    cin >> choice;

    switch (choice) {
        case '1':
        	system("cls");
            CreatePlaylist(playlists, currentUser);
            break;
        case '2':
        	system("cls");
            EditPlaylist(playlists, currentUser);
            break;
        case '3':
        	system("cls");
            listensongs(playlists, currentUser);
            break;
        case '4':
        	system("cls");
            SearchSongs(playlists, currentUser);
            break;
        case '5':
        	system("cls");
            PersonalizedRecommendations(playlists, currentUser);
            break;
        default:
            cout << "Invalid choice. Please enter 1, 2, 3, 4, or 5.\n";
            break;
    }
    }

bool AuthenticateUser(const string& username, const string& password) {
    ifstream userFile((username + "_user.txt").c_str());

    if (userFile) {
        string storedUsername, storedPassword;
        userFile >> storedUsername >> storedPassword;
        userFile.close();

        return (storedUsername == username && storedPassword == password);
    }

    return false;
}




int main() {
    string username, password;

    cout << "Welcome to the Music Player App!\n";

    // Ask for username and password
    cout << "Enter your username: ";
    cin >> username;

    cout << "Enter your password: ";
    cin >> password;

    // Authenticate user
    if (AuthenticateUser(username, password)) {
        // User authenticated successfully
        cout << "Authentication successful. Welcome, " << username << "!\n";

        stack<Playlist> playlistsStack;

        // Load playlist data from the file
        LoadData(playlistsStack, User(username, password));

        // User menu
        userMenu(playlistsStack, User(username, password));

        // Save playlist data before exiting
        SaveData(playlistsStack, User(username, password));

        cout << "Thank you for using the Music Player App!\n";
    } else {
        // User not found, ask to create an account
        cout << "User not found. Do you want to create an account? (Y/N): ";
        char choice;
        cin >> choice;

        if (choice == 'Y' || choice == 'y') {
            cout << "Enter a new password for the account: ";
            cin >> password;

            // Create a new user and save to file
            User newUser(username, password);
            newUser.SaveUserToFile();

            cout << "Account created successfully. Welcome, " << username << "!\n";

            // Initialize an empty playlist stack
            stack<Playlist> playlistsStack;

            // Load playlist data from the file (which will be empty for a new user)
            LoadData(playlistsStack, newUser);

            // User menu
            userMenu(playlistsStack, newUser);

            // Save playlist data before exiting
            SaveData(playlistsStack, newUser);

            cout << "Thank you for using the Music Player App!\n";
        } else {
            cout << "Goodbye!\n";
        }
    }

    return 0;
}



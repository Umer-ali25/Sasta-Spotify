# Sasta-Spotify
Concepts Used
    • Linked Lists
    • Stacks
    • Queue
    • Mapping
    • Searching

Project Description
A music playing application where the user can create their account via their own ID and password. User can make personalized playlists according to their music taste and classify them according to the genre of the song. User can play the music in the specific playlist as well as remove songs from the playlist.

Time Complexity
    • Authentication (AuthenticateUser function):
          Opening and reading a file: O(1) (assuming the file size is constant)
    • Loading Data (LoadData function):
    • Nested loops iterating through playlists and songs: O(N*M), where N is the number of playlists and M is the average number of songs in each playlist
    • Creating a Playlist (CreatePlaylist function):
    • Iterating through a loop to add songs to a playlist: O(numSongs)
    • Listening to Songs (listensongs function):
    • -Iterating through a loop to play songs: O(numSongs
    • Searching Songs (SearchSongs function):
    • -Nested loops searching for songs by genre or artist: O(N*M), similar to the  LoadData function
    • Editing a Playlist (EditPlaylist function):
    • -Nested loops iterating through songs in a playlist: O(N*M)
    • User Menu (userMenu function):
    • -Switch statement with constant time operations: O(1)
    • Personalized Recommendations (PersonalizedRecommendations function):
    • -Nested loops iterating through playlists and songs: O(N*M)

Overall, the time complexity of the program is influenced by the number of playlists (N) and the average number of songs in each playlist (M). In many cases, the dominant factor will be the loops iterating through playlists and songs.
The worst-case time complexity is approximately O(N*M), where N is the number of playlists, and M is the average number of songs in each playlist. It's important to note that this is a simplified analysis, and the actual performance may vary based on the specific operations within the loops and the input data.



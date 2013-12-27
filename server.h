/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct Server_T *Server_T;

// Create a server object
Server_T create_server(const char *);

// Destroy the server object
void destroy_server(Server_T *);

// Listens for incomin connections
void server_run(Server_T, unsigned int);

#endif /* _SERVER_H_ */

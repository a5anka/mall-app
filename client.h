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

#ifndef _CLIENT_H_
#define _CLIENT_H_

typedef struct Client_T *Client_T;

// Create a client object
Client_T create_client(const char *);

// Destroy the client object
void destroy_client(Client_T *);

// Asks for updated list of locations
void send_location_index_request(Client_T);

// Listens for incomin connections
void client_run(Client_T, unsigned int);

#endif /* _CLIENT_H_ */

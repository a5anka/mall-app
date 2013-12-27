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

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

#define MALL_CCNX_PREFIX "/mall"

int main(int argc, char *argv[])
{
  Server_T server;
  server = create_server(MALL_CCNX_PREFIX);

  if (server == NULL)
    {
      printf("Cannot create server!\n");
      return EXIT_FAILURE;
    }

  for (;;) {
    printf("Server listening...\n");
    server_run(server, 1000);
  }

  destroy_server(&server);

  return EXIT_SUCCESS;
}
